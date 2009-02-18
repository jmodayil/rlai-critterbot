#ifndef CritterRewardDrop_H
#define CritterRewardDrop_H

#include "Drop.h"
#include "CritterDrop.h"

using namespace std;

/**
 * Class defining the RewardDrop used by the Critterbot.
 */
class CritterRewardDrop : public CritterDrop {
 public:
	static string name;

  double reward;
  
	CritterRewardDrop();
	virtual ~CritterRewardDrop();

	virtual int getSize();
	virtual void writeArray(void *d);
	virtual void readArray(void *d);
	virtual string toString(string offset = "");
	
  virtual string getName();

};

#endif
