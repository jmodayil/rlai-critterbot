#ifndef CritterLogTagDrop_H
#define CritterLogTagDrop_H

#include "Drop.h"
#include "CritterDrop.h"

using namespace std;

/**
 * Drop to hold tag data for the Critterbot log files.
 */
class CritterLogTagDrop : public CritterDrop {
 public:
	static string name;
  
	CritterLogTagDrop();
	virtual ~CritterLogTagDrop();

	virtual int getSize();
	virtual void writeArray(void *d);
	virtual void readArray(void *d);
	virtual string toString(string offset = "");
	
  string tagInfo;

  virtual string getName() { return name; }
};

#endif