#!/usr/bin/python

# An investigation into simple python access to the critterbot/disco
# devices through the an interface similar to Thomas' java interface 

import socket
import struct
import array
import time

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
    VoltageMode=2
    XYTMode=1
    VelocityMode=0
    # The Vref commands are by Mike S. to reset after sensor shifts
    # I don't think they should be used casually. 
    MotorExit=0x68
    AVREnableChargingMode=0x69
    AVRDisableChargingMode=0x6A
    # self.AVREnableVref=0x6B
    # self.AVRDisableVref=0x6C
    AmpEnableMode=0x6D
    AmpDisableMode=0x6E

    LEDNone=0
    LEDClear=1
    LEDCustom=7
    LEDExit=0x67


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
        self.control_drop=Drop("CritterControlDrop", self.action_description)
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


#def crittersim():
#    return critter(hostname="localhost",port=2324)
#def critterbot():
#    return critter(hostname="10.0.1.20",port=2330) #From Disco


import sys

success=False
bindings={ "SoundEnable": {"MotorMode": critter.AmpEnableMode},
           "SoundDisable": {"MotorMode": critter.AmpDisableMode},
           "ChargeEnable": {"MotorMode":critter.AVREnableChargingMode},
           "ChargeDisable": {"MotorMode":critter.AVRDisableChargingMode},
           "UIMode": {"MotorMode":critter.MotorExit, "LEDMode":critter.LEDExit}
}

if len(sys.argv) > 1:
    command=sys.argv[1]
    if bindings.has_key(command):
        print command
        c=critter("10.0.1.10",2330)
       
        c.act( bindings[command]);
        print "Sent", command, bindings[command]
        success=True
        #Send commands twice as data sometimes get dropped.
        c.act( bindings[command]);
        print "Sent", command, bindings[command]
        print c.fresh_observation()

if not success:
    print "Run with one of the following command line arguments: ",bindings.keys()


