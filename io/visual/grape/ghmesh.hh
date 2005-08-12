#ifndef __GRAPE_HMESH_H__
#define __GRAPE_HMESH_H__

enum { MAX_NAME_LENGTH = 32 };

/* interface element */
typedef struct dune_elem 
{
  /* 
   *  see g_eldesc.h for ElementType 
   */
  int       type; 

  double          vpointer [MAX_EL_DOF][3];
  int             vindex [MAX_EL_DOF] ;
  int             bnd [MAX_EL_FACE] ;
  int             eindex;
  int             level;
  int             level_of_interest;
  int             has_children;
  /* is the pointer to LevelIterator or to LeafIterator */
  void          * liter;
  void          * hiter;

  /* points to actual iterator to compare an get type */
  void          * actElement;

  void          * display;
  
  void          * mesh; 

  int isLeafIterator;

} DUNE_ELEM ;


typedef struct dune_fdata 
{
  /* my number in the data vector */
  int mynum; 
  
  const char * name;
  
  /* pointer to object of discrete function */
  const void *discFunc;

  /* pointer to local function */
  const void *lf;

  /* are all Levels occupied? */
  int allLevels; 

  /* dimension of value */
  int dimVal;
  
  /* polynonial order of basis functions */
  int polyOrd; 
  
  /* continuous or not */
  int continuous;

  /* index of current component */
  int component;

  /* max number of components */
  int compName;
  
} DUNE_FDATA ;

/* setup hmesh with given data */
extern void *hmesh( 
          int (* const f_leaf)(DUNE_ELEM *), int (* const n_leaf)(DUNE_ELEM *),
          int (* const f_mac)(DUNE_ELEM *), int (* const n_mac)(DUNE_ELEM *),
          int (* const f_chi)(DUNE_ELEM *), int (* const n_chi)(DUNE_ELEM *),
          void * (* const cp)(const void *),
          int  (* const check_inside)(DUNE_ELEM *, const double *),
          int  (* const wtoc)(DUNE_ELEM *, const double *, double *),
          void (* const ctow)(DUNE_ELEM *, const double *, double *),
          void (* const func_real) (DUNE_ELEM *, DUNE_FDATA*, int ind, const double *coord,  double *),
          const int noe, const int nov, const int maxlev,int partition,
          DUNE_ELEM *he , DUNE_FDATA * fe);

extern void displayTimeScene(INFO * info);
extern void handleMesh (void *hmesh);

extern DUNE_FDATA * extractData (void *hmesh , int num );

extern void timeSceneInit(INFO *info, int n_info, int procs , int time_bar);
/* setup TimeScene Tree  */
extern void addDataToHmesh(void  *hmesh, DUNE_FDATA * fe,
    void (* const func_real) (DUNE_ELEM *, DUNE_FDATA*, int ind, const double *, double *)  );
/* setup TimeScene Tree  */
extern void addHmeshToTimeScene(void * timescene, double time, void  *hmesh , int proc);
extern void addHmeshToGlobalTimeScene(double time, void  *hmesh , int proc);
extern void tsc_timebar(void *timescene, double t_start, double t_end);

#endif
