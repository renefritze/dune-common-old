#ifndef __GRAPE_COMMON_CC__
#define __GRAPE_COMMON_CC__

#define MINIMUM(a,b) (((a) > (b)) ? (b) : (a))
#define MAXIMUM(a,b) (((a) < (b)) ? (b) : (a))

#include "grapecommon.hh"

/* add Button which can switch between LevelIteration and LeafIteration */
void setupLeafButton(MANAGER *mgr, void *sc, int yesTimeScene)
{
  assert(!leafButton);
  assert(!maxlevelButton);

  leafButton = (BUTTON *)
      new_item (Button,
      I_Label, "LeafIterator",
      I_Instance, sc,
      I_Method, "leaf-button-on-off",
      I_Size, 12., 1.,
      I_FillMode, MENU_FILL_BOTTOM,
      I_End);

  maxlevelButton = (BUTTON *)
      new_item (Button,
      I_Label, "use maxlevel",
      I_Instance, sc,
      I_Method, "maxlevel-on-off",
      I_Size, 12., 1.,
      I_FillMode, MENU_FILL_BOTTOM,
      I_End);

  GRAPE(mgr,"add-inter")(leafButton);
  GRAPE(mgr,"add-inter")(maxlevelButton);

  GRAPE(leafButton,"set-state")(PRESSED);
  leafButton->on_off = OFF;
  GRAPE(maxlevelButton,"set-state")(PRESSED);
  maxlevelButton->on_off = OFF;
}


void timeSceneInit(INFO *info, int n_info, int procs, int time_bar)
{
  int   n,p;
  int numProcs = (procs <= 1) ? 1 : (procs-1);
#ifdef TIMEBAR
  void def_timebar_methods(void);

  if (time_bar)
    def_timebar_methods();
#endif

  for (n = 0; n < MAXIMUM(1, n_info); n++)
  {
    printf("n = %d, make TimeScene \n",n);
    info[n].tsc = (TIMESCENE *)GRAPE(TimeScene,"new-instance")(info[n].name);
    ((TIMESCENE *)info[n].tsc)->sync = 1;
    if (n > 0)
      ((TIMESCENE *) info[n-1].tsc)->next_scene = (SCENE *) info[n].tsc;
  }

  for (n = 0; n < MAXIMUM(1, n_info); n++)
  {
    TIMESCENE * tsc = (TIMESCENE *) info[n].tsc;
    // > 0 because tsc for proc 0 already exists 
    for (p = numProcs-1; p > 0; p--)
    {
      TIMESCENE * newSc = NULL;
      printf("add timescene for proc %d \n",p);
      assert(tsc);
      char * newName = (char *) malloc(strlen(info[n].name) + 5 * sizeof(char));
      assert(newName);
      sprintf(newName,"%s_%d",info[n].name,p);
      newSc = (TIMESCENE *)GRAPE(TimeScene,"new-instance")(newName);
      assert(newSc);

      newSc->sync = 1;
      newSc->next_scene = tsc->next_scene;
      tsc->next_scene = (SCENE *) newSc;
    }
  }

  return;
}

/* call handle for a bunch of timescenes */
void displayTimeScene(INFO * info, int numberOfProcs )
{
  TIMESCENE *tsc = (TIMESCENE *) info[0].tsc;
  if(tsc)
  {
    MANAGER       *mgr;
#ifdef GRID_MODE
    GRAPHICDEVICE *grdev;

    grdev = (GRAPHICDEVICE *)GRAPE(GraphicDevice,"get-stddev")();
    grdev->clear();
    if (grdev && (grdev->grid_patch != G_GRID))
    {
      GRAPE(grdev,"grid-patch")(G_GRID);
    }
#endif
    GrapeInterface_two_two::grape_add_remove_methods();
    GrapeInterface_three_three::grape_add_remove_methods();
    GrapeInterface_three_three::initPartitionDisp(numberOfProcs-1);
    
    mgr = (MANAGER *)GRAPE(Manager,"get-stdmgr")();

    if((!leafButton) || (!maxlevelButton))
      setupLeafButton(mgr,tsc,1);

    GRAPE(mgr,"handle")(tsc);
  }
}

#ifdef TIMEBAR
void tsc_timebar(void *timescene, double t_start, double t_end)
{
  TIMESCENE *tsc = (TIMESCENE *) timescene;
  GRAPE(tsc,"timebar-v")(t_start, t_end);
}

#include "tsc_timebar.c"
#endif

#undef MINIMUM
#undef MAXIMUM 

#endif
