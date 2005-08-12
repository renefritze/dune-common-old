#ifndef __GRAPE_COMMON_H__
#define __GRAPE_COMMON_H__

/* global variables for leaf iterator display */
static BUTTON * leafButton=0;
/* global variables for maxlevel use */
static BUTTON * maxlevelButton=0;

static TIMESCENE * globalTsc = 0;

/* info about data on one mesh */
typedef struct datainfo DATAINFO;
struct datainfo
{
  int   vector;   /*  0: REAL 1: REAL_D */
  const char  *name;
  DATAINFO *next;
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
