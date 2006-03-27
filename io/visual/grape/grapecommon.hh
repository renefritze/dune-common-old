#ifndef DUNE_GRAPECOMMON_HH
#define DUNE_GRAPECOMMON_HH

/* global variables for maxlevel use */
static BUTTON * maxlevelButton=0;

/* on click set min and max value of function to colorbar */
static BUTTON * minMaxColorbar=0;

/* global variables for iterator choice */
static COMBOBUTTON  * iteratorButton = 0;

/* global variables for partition type choice */
static COMBOBUTTON * partitionTypeButton = 0;

static TIMESCENE * globalTsc = 0;

/* info about data on one mesh */
typedef struct datainfo DATAINFO;
struct datainfo
{
  const char * name;
  const char * base_name;
  DATAINFO *next;

  int dimVal; /* length of vector (dimVal = 1 --> scalar, otherwise vector  */
  int * comp; /* number of each component */
};

/* info about one mesh */
typedef struct info INFO;
struct info
{
  int fix_mesh; /* if no dynamic grid 1 : else 0 */
  const char  *name;
  DATAINFO * datinf;
  void  *tsc;
};

void setupLeafButton(MANAGER *mgr, void *sc, int yesTimeScene);

#endif
