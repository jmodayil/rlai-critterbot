# An investigation into simple python access to the critterbot/disco
# devices through the an interface similar to Thomas' java interface 

import socket
import struct
import array

#---------------- Drop Interface
class Dconst:
    int_size=4
    char_size=1
    byte_size=1
    time_size=8
    num_leds=16
    max_voltage=25

def struct_grab(fmt,buffer,offset):
    return struct.unpack(fmt,buffer[offset:offset+struct.calcsize(fmt)])[0]

class DInt:
    "A Drop Int"
    def __init__(self,name,value=None):  
        self.name=name
        self.value=value
        if self.value!=None:
            self.read_only=False
        else:
            self.read_only=True
    def size(self):
        return Dconst.int_size
    def pack(self,buffer,offset,value):
        struct.pack_into("i",buffer,offset,value)
    def unpack(self,buffer,offset):
        return struct_grab("i",buffer,offset)


class DArray:
    """A Drop array of integers by default"""
    def __init__(self,name,vals,prototype=None):
        self.name=name
        self.read_only=False
        if prototype==None:
            prototype=DInt("",0)
        self.prototype=prototype        
        if type(vals)==type(1): #default value
            self.names=["%d"%x for x in range(vals)]
            self.n=vals
        else:  #or names... this is messy & not good python.
            self.names=vals
            self.n=len(vals)
        self.value=self.n*[self.prototype.value]
    def size(self):
        return self.n*self.prototype.size()
    def pack(self,buffer,offset,value):        
        #print value
        for i in range(self.n): #x,v in zip(self.data_description,value):
            self.prototype.pack(buffer,offset,value[i])
            offset+=self.prototype.size()
    def unpack(self,buffer,offset):
        vals=[]
        for i in range(self.n):
            vals.append(self.prototype.unpack(buffer,offset))
            offset+=self.prototype.size()
        return tuple(vals)

class DTime:
    "A Drop Timestamp is in milliseconds, and stored in a long"
    def __init__(self):  #What is a default value for time?
        self.name="Time"
        self.read_only=False
    def size(self):
        return Dconst.time_size
    def pack(self,buffer,offset,value):
        struct.pack_into("L",buffer,offset,value)
    def unpack(self,buffer,offset):
        v1=struct_grab("L",buffer,offset)
        #v2=struct_grab("L",buffer,offset+4) #data not sent
        #print v1,v2
        return v1


class DString:
    def __init__(self,name,value="",read_only=False):
        self.name=name
        self.value=value
        self.read_only=read_only
    def size(self):
        return len(self.value)*Dconst.char_size+ Dconst.int_size
    def pack(self,buffer,offset,value):
        struct.pack_into("i%ds"%len(value),buffer,offset,len(value),value)
    def unpack(self,buffer,offset):
        sz=struct_grab("i",buffer,offset)
        str=struct_grab("%ds"%sz,buffer,offset+Dconst.int_size)
        return str

class DColor:
    def __init__(self,name,value=(255,255,0),read_only=False):
        self.name=name
        self.value=value
        self.read_only=read_only
    def size(self):
        return 3*Dconst.byte_size
    def pack(self,buffer,offset,value):
        r,g,b=value
        struct.pack_into("ccc",buffer,offset,chr(r),chr(g),chr(b))
    def unpack(self,buffer,offset):
        r,g,b=struct.unpack("ccc",buffer[offset:offset+3])
        return ord(r),ord(g),ord(b)


class Drop:
    def __init__(self,name,data_description):
        self.known={}
        self.buffer=[]
        self.name=DString("Name",name)
        self.data_description=data_description
        self.data_size=sum(x.size() for x in data_description) 
        self.header_size=self.name.size()+Dconst.int_size #int/name/rest
        self.packet_size=self.header_size+self.data_size
        buffer=array.array('c','\0'*self.header_size)
        self.name.pack(buffer,0,self.name.value)
        offset=self.name.size()
        struct.pack_into("i",buffer,offset,self.data_size)
        self.header=buffer.tostring()
    def pack(self):
        offset=0
        buffer=array.array('c','\0'*self.packet_size)
        self.name.pack(buffer,offset,self.name.value)
        offset+=self.name.size()
        struct.pack_into("i",buffer,offset,self.data_size)
        offset+=Dconst.int_size
        for x in self.data_description:
            if x.read_only:
                value=x.value
            elif x.name in self.known:
                value=self.known[x.name]
            else:
                value=x.value
            #print x
            x.pack(buffer,offset,value) 
            offset+=x.size()
        return buffer
    def unpack(self,buffer):
        offset=self.header_size
        for x in self.data_description:
            vals=x.unpack(buffer,offset)
            self.known[x.name]=vals
            offset+=x.size()
        return self.known
#---------------- Critterbot interface

def int_or_list_mean(vals):
    n=float(len(vals))
    if type(vals[0])==type(0):
        return sum(vals)/n
    k=len(vals[0])
    return tuple([sum(v[i] for v in vals)/n for i in range(k)])

import string

#ok on simulator, but there is extra junk on the socket with the real robot, namely control drops coming in.                     
PACKET_DEBUG=False
class critter(object):
    def __init__(self,hostname,port): 
        # Open port
        self.sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.sock.connect((hostname,port))

        motor_names=("Command","Speed","Current","Temperature")
        axis_names=("X","Y","Z")
        self.observation_description=[ 
            DInt("Datasource"),
            DTime(),
            DInt("PowerSource"),
            DInt("ChargeState"),
            DInt("BusVoltage"),
            DArray("Bat",3),
            DArray("Motor0", motor_names),
            DArray("Motor1", motor_names),
            DArray("Motor2", motor_names),
            DArray("Accel", axis_names),
            DArray("Mag", axis_names),
            DInt("RotationVel"),
            DArray("IR",10),
            DArray("IRLight",8),
            DArray("Light",4),
            DArray("Thermal",8),
            DArray("Bump",32),
            DInt("ErrorFlags"),
            DInt("CycleTime"),
            DInt("RealRobotExcess")
            ]
        self.VoltageMode=2
        self.XYTMode=1
        self.VelocityMode=0

        self.action_description=[ 
            #Use motor mode 1 for (vx,vy,vth), 0 for wheel velocities
            DInt("MotorMode", 1), 
            #DArray("VelocityCommand", ("M100", "M220", "M340")),
            DArray("Command", ("M100", "M220", "M340")),
            DInt("LedMode", 0), 
            DArray("Led", Dconst.num_leds,DColor("")) #This would be a problem
            ]

        self.observation_drop=Drop("CritterStateDrop", self.observation_description)
        self.control_drop=Drop("CritterControlDrop", self.action_description)
        self.motor_range=3*[(-Dconst.max_voltage,Dconst.max_voltage)]
        self.last_time=time.time()
    def __del__(self):
        #Close port
        self.sock.close()

    def act(self,command_dict):
        #Send command
        for k,v in command_dict.items():
            self.control_drop.known[k]=v
        buffer=self.control_drop.pack()
        #print "Command Buffer",buffer
        self.sock.send(buffer)
    def new_observation(self):
        #We can ensure that the data is not too stale by ensuring that we have waited for at least10ms before responding.
        start_time=time.time()
        while True:
            out=self.observe()
            end_time=time.time()
            if end_time-start_time > .01:
                return out
    def fresh_observation(self,period=.01):
        #We can ensure that the data is not too stale by ensuring that we have waited for at least10ms before responding.
        while True:
            out=self.observe()
            end_time=time.time()
            if end_time-self.last_time >= period:
                self.last_time=end_time
                return out

    def avg_fresh_observation(self,period=.01):
        #We can ensure that the data is not too stale by ensuring that we have waited for at least10ms before responding.
        store=[]
        while True:
            out=self.observe()
            store.append(out.copy())
            end_time=time.time()
            if end_time-self.last_time >= period:
                new_out={}
                keys=store[0].keys()
                for k in keys:
                    vals=[s[k] for s in store]
                    new_out[k]=int_or_list_mean(vals)
                new_out["Time"]=store[-1]["Time"]
                new_out["Datasource"]=store[-1]["Datasource"]
                self.last_time=end_time
                return new_out

    def observe(self):
        #Read observation
        data=""
        while True:
            delta=self.observation_drop.packet_size-len(data)
            got=self.sock.recv(delta)
            data+=got
            if got=="":    #broken connection
                return ""
            #Now check that the header is right, otherwise trim data appropriately
            if PACKET_DEBUG:
                print "delta,len,data:",delta,len(data),data
            head=self.observation_drop.header
            offset=string.find(data,head)
            if (offset!=0):
                if PACKET_DEBUG:
                    print "Junk in the socket to ",offset,", Moving along."
                    print " looking for: ",head
                    print " seeing:",data[:len(head)]
                if offset<0:
                    data=data[-len(head):]
                else:
                    data=data[offset:]
            if len(data)== self.observation_drop.packet_size:
                break
            if PACKET_DEBUG:
                print "Grabbing more data from socket..."
        observation=self.observation_drop.unpack(data)
        return observation


def crittersim():
    return critter(hostname="localhost",port=2324)
def critterbot():
    return critter(hostname="10.0.1.20",port=2330) #From Disco
#-----------------------------

class generic_drops(object):
    def __init__(self,hostname,port, name, description):

        # Open port
        self.sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.sock.connect((hostname,port))

        self.observation_description=name
        self.action_description=description
        self.observation_drop=Drop(name, description)
    def __del__(self):
        self.sock.close()

    def act(self,command_dict):
        for k,v in command_dict.items():
            self.control_drop.known[k]=v
        buffer=self.control_drop.pack()
        self.sock.send(buffer)
    def new_observation(self):
        #We can ensure that the data is not too stale by ensuring that we have waited for at least10ms before responding.
        start_time=time.time()
        while True:
            out=self.observe()
            end_time=time.time()
            if end_time-start_time > .01:
                return out

    def observe(self):
        #Read observation
        data=""
        while True:
            delta=self.observation_drop.packet_size-len(data)
            got=self.sock.recv(delta)
            data+=got
            if got=="":    #broken connection
                return ""
            #Now check that the header is right, otherwise trim data appropriately
            if PACKET_DEBUG:
                print "delta,len,data:",delta,len(data),data
            head=self.observation_drop.header
            offset=string.find(data,head)
            if (offset!=0):
                if PACKET_DEBUG:
                    print "Junk in the socket to ",offset,", Moving along."
                    print " looking for: ",head
                    print " seeing:",data[:len(head)]
                if offset<0:
                    data=data[-len(head):]
                else:
                    data=data[offset:]
            if len(data)== self.observation_drop.packet_size:
                break
            if PACKET_DEBUG:
                print "Grabbing more data from socket..."
        observation=self.observation_drop.unpack(data)
        return observation

SoundFft="FftAudioDrop"
SoundFftFeatures=[DArray("Left",9),DArray("Right",9)]
WalterIP="10.0.1.10"

def getFftSound(): return generic_drops(WalterIP,7001,SoundFft,SoundFftFeatures)

def SendSound(): return generic_drops(WalterIP,7003,"AudioDrop",[DArray("s",4400)])

SoundRawFeatures=[DArray("Left",1024),DArray("Right",1024)]
def getRawSound(): return generic_drops(WalterIP,7002,"AudioDrop",SoundRawFeatures)
#-----------------------------

class raw_logs(object):
    #write/read a log with int (tupled) dictionaries.  Still messy
    def __init__(self,file,write=True):
        self.f=open(file,"w")
        self.known={}
    def __del__(self):
        self.f.close()
    def register(self,kind,sample):
        if kind in known:
            return
        id=len(known)
        self.f.write("# %d "%id +kind)
        access=[]
        for name,values in sample.items():
            if type(values)==type(0):
                f.write(" %s."%(name))
                access.append((name,None))
            else:
                f.write(" %s_%d"%(name,len(values)))
                for i in range(len(values)):
                    f.write(" %s%d"%(name,i))
                    access.append((name,i))
        known[kind]=sample
        f.write("\n")
    def write(self,kind,sample):
        id,access=known[kind]
        f.write("%d"%id)
        for n,v in access:
            if v:
                f.write(" %d",sample[n][v])
            else:
                f.write(" %d",sample[n])
        f.write("\n")
    def read(self):
        
        line=self.f.readline()
        if len(line)==0:
            return None
        if line[0]=="#":
            vals=line.split()
            kind=vals[0]
            items=vals[1:]
            scan=[]
            for x in items:
                if x[-1]==".":
                    scan.append((x[:-1],None))
                else:
                    name,count=x.split("_")
                    scan.append((name,int(count)))
            self.known[id]=(kind,scan)
            return read()
        tmp=line.split()
        id,vals=tmp[0],tmp[1:]
        ret={}
        scan=self.known[id][1]
        index=0
        for name,count in scan:
            if count==None:
                ret[name]=vals[index]
                index+=1
            else:
                ret[name]=tuple(vals[index:index+count])
                index+=count
        return ret

#-----------------------------
import random
import time
def random_walk_to_goal(goal_accessor,goal_value,critter=None):
    last=None
    last_motor=None
    pr_repeat=.5
    if not critter:
        c=crittersim()
    else:
        c=critter
    delta=lambda x: abs(x-goal_value)
    for i in range(1000):
        o=c.observe()
        #print "Obs",o
        now=goal_accessor(o)
        if last_motor==None or (
            (random.random() > pr_repeat) and 
            (delta(now) > delta(last))):
            motor=[random.randint(x[0],x[1]) for x in c.motor_range]
            last_motor=motor
        else:
            motor=last_motor
        print "Distance to goal:",delta(now)
        s=min(1,delta(now)/100.0)
        motor=[int(s*s*x) for x in motor]
        c.act({"VelocityCommand":motor})
        time.sleep(.04)
        last=now
    return c

ir2=lambda x: x["IR"][2]
#------------------------------
def wall_follow():
    c=critterbot()
    o1=c.new_observation()
    initial_offset=time.time()-o1["Time"]*.001
    print o1    
    for i in range(500):
        o=c.new_observation()
        print "Delay (s): " , time.time()-o["Time"]*.001 - initial_offset
        #right, from back 10,5,6,7
        #Front, 1, 2,3
        #left=(o["IR"][2]+o["IR"][3]+o["IR"][4])/3
        #right=mean(o["IR"][4]-150)+(o["IR"][6])#+o["IR"][6]+o["IR"][4])/3
        front=max(o["IR"][0],o["IR"][1] )
        right_up=o["IR"][5]
        right_down=o["IR"][6]

        right_distance=(right_up+right_down)*0.5
        right_angle=right_up-right_down

        front_C=150
        side_C=150
        side_limit=40
        turn_limit=90

        turn_scale=30
        
        side_d=right_distance-side_C

        drive_speed=15
        side_speed=15
        turn_speed=10

        if front > front_C:
            motor=[0,0,turn_speed]
        elif (side_d)> side_limit:
            motor=[0,-side_speed,0]
        elif (side_d)< -side_limit:
            motor=[0,side_speed,0]
        #elif right_angle>turn_limit:
        #    motor=[0,0, turn_limit/turn_scale]
        #elif right_angle>-turn_limit:
        #    motor=[0,0, -turn_limit/turn_scale]
        else:
            motor=[-drive_speed,0,0]#right_angle/turn_scale]
        #print "\nRAW:",front, right_up,right_down
        print "\nfront, side, M+(angle):",front,side_d,right_angle
        #for i in range(len(o["IR"])):
        #    print "     %d=%d"%(i,o["IR"][i]),
        print "Motor",motor
        c.act({"XYTMode":c.VelocityMode, "Command":motor})
        time.sleep(.001)
    print o["Time"]
    del(c)

    #there is a substantial latency to be measured.

#random_walk_to_goal(ir2,50) # Babble until the back is pointing at something.

#This is probably a good point to factor the code.  Disco, Critter,
#Controllers Given multiple observers, we wish to learn options, and
#c(s,a) = recognizers => policies \pi(s,a) = c(s,a) *b(s,a)/Z from
#past behavior b(s,a).  Described as such in a paper with Rich and Duane.
