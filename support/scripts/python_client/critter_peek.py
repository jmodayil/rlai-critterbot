#!/usr/bin/python
# An investigation into simple python access to the critterbot/disco
# devices through the an interface similar to Thomas' java interface 

#---------------- Critterbot interface

import critter as C

c=C.critter("10.0.1.10",2330)
v=c.observe().items()
v.sort()
print v

