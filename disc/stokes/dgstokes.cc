#ifdef HAVE_CONFIG_H
# include "config.h"     // autoconf defines, needed by the dune headers
#endif
#include "dgstokes.hh"
#include"testfunctions.hh"




template<class G,int ordr>
void DGStokes<G,ordr>::assembleVolumeTerm(Entity& epointer, LocalMatrixBlock& Aee,LocalVectorBlock& Be) const
{
  Gradient grad_phi_ei[dim],grad_phi_ej[dim],temp;
  ctype  phi_ei, phi_ej,psi_ei,psi_ej;
  ctype entry;
  
  //get the shape function set
  ShapeFunctionSet vsfs(ordr);; //for  velocity
  ShapeFunctionSet psfs(ordr-1); // for pressure
  //shape function size and total dof
  int vdof=vsfs.size()*2; // two velocity components and total velocity sfs size
  int pdof=psfs.size();
  //get parameter
  DGStokesParameters parameter;
  
  //get the geometry type
  Dune::GeometryType gt = *epointer.geometry().type();
  //specify the quadrature order ?
  int qord=18;
  for (int nqp=0;nqp<Dune::QuadratureRules<ctype,dim>::rule(gt,qord).size();++nqp) 
	{
	  //local position of quad points
	  const Dune::FieldVector<ctype,dim> & quad_point_loc = Dune::QuadratureRules<ctype,2>::rule(gt,qord)[nqp].position();
	  //global position
	  Dune::FieldVector<ctype,dim> quad_point_glob = *epointer.geometry().global(quad_point_loc); 
	  // calculate inv jacobian
	  InverseJacobianMatrix inv_jac=*epointer.geometry().jacobianInverseTransposed(quad_point_loc);
	  // quadrature weight 
	  double quad_wt=Dune::QuadratureRules<ctype,dim>::rule(gt,qord)[nqp].weight();
	  // get the determinant jacobian
	  ctype detjac=*epointer.geometry().integrationElement(quad_point_loc);


	  //================================================//
	  // source term: TERM:14 : f* v
	  // TERM 1 : \int (mu*grad_u*grad_v)
	  //================================================//	  
	  // dimension - 2 velocity comps. (now assumming dim==2)
	  for(int dm=1;dm<=2;++dm) 
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  //space dimension-sd  (now assumed dim==2)
			  for (int sd=0; sd<2; sd++)
				temp[sd] = vsfs[i].evaluateDerivative(0,sd,quad_point_loc);
			  grad_phi_ei[dm-1] = 0;
			  //matrix vect multiplication
			  // transform gradient to global coordinates by multiplying with inverse jacobian
			  inv_jac.umv(temp,grad_phi_ei[dm-1]);
			  int ii=(dm-1)*vsfs.size()+i; 
			  double f;
			  RHS rhs;
			  if (dm==1)
				{
				  rhs.u_rhs(quad_point_glob[0],quad_point_glob[1],f);
				}
			  else
				{
				  rhs.v_rhs(quad_point_glob[0],quad_point_glob[1],f);
				}
			  Be[ii]+=f*vsfs[i].evaluateFunction(0,quad_point_loc)*detjac*quad_wt;
				 
			  for (int j=0;j<vsfs.size();++j)
				{
				  for (int sd=0; sd<2; sd++)//space dimension -sd
					temp[sd] = vsfs[j].evaluateDerivative(0,sd,quad_point_loc);
				  grad_phi_ej[dm-1] = 0;
				  inv_jac.umv(temp,grad_phi_ej[dm-1]);
				  int jj=(dm-1)*vsfs.size()+j;
				  entry =parameter.mu*(grad_phi_ei[dm-1]*grad_phi_ej[dm-1])*detjac*quad_wt;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  //================================================//	
	  // -  p * div v 
	  //================================================//
	  for(int dm=1;dm<=2;++dm)//(now assumming dim==2)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i;
			  for (int sd=0; sd<2; sd++)//space dimension -sd
				temp[sd] = vsfs[i].evaluateDerivative(0,sd,quad_point_loc);
			  grad_phi_ei[dm-1] = 0;
			  inv_jac.umv(temp,grad_phi_ei[dm-1]);
			  for (int j=0;j<psfs.size();++j) // pressure shapefns
				{
				  int jj=vdof+j;
				  psi_ej=psfs[j].evaluateFunction(0,quad_point_loc);
				  entry =-(grad_phi_ei[dm-1][dm-1]*psi_ej)*detjac*quad_wt;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  //================================================//	
	  // 	 -  q* div u 	
	  //================================================//			  	  
	  for(int dm=1;dm<=2;++dm)//(now assumming dim==2)
		{
		  for (int i=0;i<psfs.size();++i) // pressure shapefns
			{
			  int ii=vdof+i;
			  psi_ei=psfs[i].evaluateFunction(0,quad_point_loc);
			  //if(i==0) psi_ei=0.0;
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  for (int sd=0; sd<2; sd++)//space dimension -sd
					temp[sd] = vsfs[j].evaluateDerivative(0,sd,quad_point_loc);
				  grad_phi_ej[dm-1] = 0;
				  inv_jac.umv(temp,grad_phi_ej[dm-1]);
				  entry =-(grad_phi_ej[dm-1][dm-1]*psi_ei)*detjac*quad_wt;
				  Aee.add(ii,jj,entry);
				  //std::cout<<"AA(): "<<AA(ig,jg)<<std::endl;
				}
			}
		}
	  //================================================//	

	} // end of volume term quadrature
}// end of assemble volume term



template<class G,int ordr>
void DGStokes<G,ordr>::assembleFaceTerm(Entity& epointer, IntersectionIterator& ispointer, LocalMatrixBlock& Aee, LocalMatrixBlock& Aef,LocalMatrixBlock& Afe, LocalVectorBlock& Be) const
{
  Gradient grad_phi_ei[dim],grad_phi_ej[dim],grad_phi_fi[dim],grad_phi_fj[dim],temp;
  ctype   phi_ei[dim],phi_ej[dim],phi_fi[dim],phi_fj[dim],psi_ei,psi_ej,psi_fi,psi_fj;
  ctype entry;
//get the shape function set
  //self shape functions
  ShapeFunctionSet vsfs(ordr);; //for  velocity
  ShapeFunctionSet psfs(ordr-1); // for pressure
  //neighbor shape functions
  ShapeFunctionSet nbvsfs(ordr);; //for  velocity
 
  //shape function size and total dof
  int vdof=vsfs.size()*2; // two velocity components and total velocity sfs size
  int pdof=psfs.size();
  //get parameter
  DGStokesParameters parameter;
  //get the geometry type of the face
  Dune::GeometryType gtface = *ispointer.intersectionSelfLocal().type();
  //specify the quadrature order ?
  int qord=18;

  for (int qedg=0; qedg<Dune::QuadratureRules<ctype,dim-1>::rule(gtface,qord).size(); ++qedg)
	{	
	  //quadrature position on the edge/face in local=facelocal
	  const Dune::FieldVector<ctype,dim-1>& local = Dune::QuadratureRules<ctype,dim-1>::rule(gtface,qord)[qedg].position();
	  Dune:: FieldVector<ctype,dim> face_self_local = *ispointer.intersectionSelfLocal().global(local);
	  Dune:: FieldVector<ctype,dim> face_neighbor_local = *ispointer.intersectionNeighborLocal().global(local);
	  Dune::FieldVector<ctype,2> global = *ispointer.intersectionGlobal().global(local);
	  // calculating the inverse jacobian check if it is correct
	  InverseJacobianMatrix inv_jac= *epointer.geometry().jacobianInverseTransposed(face_self_local);
	  // get quadrature weight
	  double quad_wt_face = Dune::QuadratureRules<ctype,dim-1>::rule(gtface,qord)[qedg].weight();
	  ctype detjacface = *ispointer.intersectionGlobal().integrationElement(local);
	  // get the face normal-- unit normal.
	  Dune::FieldVector<ctype,dim> normal = *ispointer.unitOuterNormal(local);
	  double norm_e= *ispointer.intersectionGlobal().integrationElement(local);

	  //================================================//	
	  // term to be evaluated : TERM:2
	  //- \mu \int average(\nabla u). normal . jump(v) 
	  //================================================//	
	  // diagonal block
	  // -mu* 0.5 * grad_phi_ei * normal* phi_ej 
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i;
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,face_self_local);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  for (int sd=0; sd<2; sd++)
					temp[sd] = vsfs[j].evaluateDerivative(0,sd,face_self_local);
				  grad_phi_ej[dm-1] = 0;
				  // transform gradient to global ooordinates by multiplying with inverse jacobian
				  inv_jac.umv(temp,grad_phi_ej[dm-1]);
				  entry =-0.5 * parameter.mu * ((grad_phi_ej[dm-1]*normal)*phi_ei[dm-1])*detjacface*quad_wt_face;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  // offdiagonal entry
	  // mu* 0.5 * grad_phi_ei * normal* phi_fj 
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<nbvsfs.size();++i) 
			{
			  int ii=(dm-1)*nbvsfs.size()+i; 
			  phi_fi[dm-1] = nbvsfs[i].evaluateFunction(0,face_neighbor_local);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j; 
				  for (int sd=0; sd<2; sd++)
					temp[sd] = vsfs[j].evaluateDerivative(0,sd,face_self_local);
				  grad_phi_ej[dm-1] = 0;
				  inv_jac.umv(temp,grad_phi_ej[dm-1]);
				  entry =  0.5*parameter.mu*((grad_phi_ej[dm-1]*normal)*phi_fi[dm-1])*detjacface*quad_wt_face;
				  Afe.add(ii,jj,entry);
				}
			}
		}
	  //================================================//	
	  // term to be evaluated TERM:4
	  // \mu \parameter.epsilon .\int average(\nabla v). normal . jump(u)
	  //================================================//	
	  // diagonal term 
	  // mu* 0.5 * parameter.epsilon* phi_ei * grad_phi_ej* normal 
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i; 
			  for (int sd=0; sd<2; sd++)
				temp[sd] = vsfs[i].evaluateDerivative(0,sd,face_self_local);
			  grad_phi_ei[dm-1] = 0;
			  inv_jac.umv(temp,grad_phi_ei[dm-1]);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,face_self_local); 
				  entry=  0.5*parameter.mu*parameter.epsilon*(phi_ej[dm-1]*(grad_phi_ei[dm-1]*normal))*detjacface*quad_wt_face;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  // offdiagonal block 
	  // -mu* 0.5 * parameter.epsilon * grad_phi_ej * normal* phi_fi 
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i; 
			  for (int sd=0; sd<2; sd++)
				temp[sd] = vsfs[i].evaluateDerivative(0,sd,face_self_local);
			  grad_phi_ei[dm-1] = 0;
			  inv_jac.umv(temp,grad_phi_ei[dm-1]);
			  // note test fns are now from neighbor element
			  for (int j=0;j<nbvsfs.size();++j) 
				{
				  int jj=(dm-1)*nbvsfs.size()+j;
				  phi_fj[dm-1] = nbvsfs[j].evaluateFunction(0,face_neighbor_local); 
				  entry =-0.5*parameter.mu*parameter.epsilon*( phi_fj[dm-1]*(grad_phi_ei[dm-1]*normal))*detjacface*quad_wt_face;
				  Aef.add(ii,jj,entry);
				}
			}
		}
	  
	  //================================================//	
	  // term to be evaluated TERM:6
	  //  term J0 =  \mu. (\parameter.sigma/norm(e)). jump(u). jump (v)
	  //================================================//	
	  // Diagonalblock :
	  // \mu. (\parameter.sigma/abs(e)).\int phi_ie. phi_je  where abs(e) =  norm (e) ???
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{ 
			  int ii=(dm-1)*vsfs.size()+i; 
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,face_self_local); 
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,face_self_local); 	 
				  entry=parameter.mu*(parameter.sigma/norm_e)*phi_ei[dm-1]*phi_ej[dm-1]*detjacface*quad_wt_face;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  // offdiagonal block
	  //- mu*(parameter.sigma/norm_e)*phi_ei*phi_fj*detjacface*quad_wt_face;
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,face_self_local);
			  int ii=(dm-1)*vsfs.size()+i; 
			  for (int j=0;j<nbvsfs.size();++j) // neighbor basis
				{
				  int jj=(dm-1)*nbvsfs.size()+j;
				  phi_fj[dm-1] = nbvsfs[j].evaluateFunction(0,face_neighbor_local);
				  entry=-parameter.mu*(parameter.sigma/norm_e)*phi_ei[dm-1]*phi_fj[dm-1]*detjacface*quad_wt_face;
				  Aef.add(ii,jj,entry);
				}
			}
		}
	  //================================================//	
	  // term to be evaluated TERM:9
	  //edge  term from B(v,p)
	  // term \int average(p). jump(v).normal
	  //================================================//	
	  //diagonal block
	  // term==  0.5 * psi_ei. phi_ej* normal
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i;
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,face_self_local);
			  for (int j=0;j<psfs.size();++j) 
				{
				  int jj=vdof+j;
				  psi_ej = psfs[j].evaluateFunction(0,face_self_local); 	
				  entry =0.5*(phi_ei[dm-1]*psi_ej*normal[dm-1])*detjacface*quad_wt_face;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  
	  //offdiagonal block
	  // term==  -0.5 * psi_ei. phi_fj* normal
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<nbvsfs.size();++i) 
			{
			  int ii=(dm-1)*nbvsfs.size()+i; 
			  phi_fi[dm-1] = nbvsfs[i].evaluateFunction(0,face_neighbor_local); 
			  for (int j=0;j<psfs.size();++j) // neighbor
				{
				  int jj=vdof+j;
				  psi_ej = psfs[j].evaluateFunction(0,face_self_local); 
				  entry = -0.5*(phi_fi[dm-1]*psi_ej*normal[dm-1])*detjacface*quad_wt_face;
				  Afe.add(ii,jj,entry);
				}
			}
		}
	  
	  
	  //================================================//	
	  // term to be evaluated TERM:12
	  //edge  term from B(q,u)
	  // term \int average(q). jump(u).normal
	  // TERM:12 
	  //================================================//	
	  //diagonal block
	  // term==  0.5 * psi_ej. phi_ei* normal
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<psfs.size();++i) 
			{
			  int ii=vdof+i;
			  psi_ei = psfs[i].evaluateFunction(0,face_self_local); 	 
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,face_self_local);
				  entry =0.5*(phi_ej[dm-1]*psi_ei*normal[dm-1])*detjacface*quad_wt_face;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  
	  //offdiagonal block
	  // term==  -0.5 * psi_ej. phi_fi* normal
	  
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<psfs.size();++i) 
			{
			  int ii=vdof+i;
			  psi_ei = psfs[i].evaluateFunction(0,face_self_local); 
			  for (int j=0;j<nbvsfs.size();++j) // neighbor
				{
				  phi_fj[dm-1] = nbvsfs[j].evaluateFunction(0,face_neighbor_local); 
				  int jj=(dm-1)*nbvsfs.size()+j; 
				  entry = -0.5*(phi_fj[dm-1]*psi_ei*normal[dm-1])*detjacface*quad_wt_face;
				  Aef.add(ii,jj,entry);
				}
			}
		}
	  
	  //================================================//
	  
	}// end of assemble face quadrature loop
  
}// end of assemble face term


template<class G,int ordr>
void DGStokes<G,ordr>::assembleBoundaryTerm(Entity& epointer, IntersectionIterator& ispointer, LocalMatrixBlock& Aee, LocalVectorBlock& Be) const
{
  Gradient grad_phi_ei[dim],grad_phi_ej[dim],temp;
  ctype   phi_ei[dim],phi_ej[dim],psi_ei,psi_ej;
  ctype entry;
  ctype dirichlet[dim];
  //get the shape function set
  //self shape functions
  ShapeFunctionSet vsfs(ordr);; //for  velocity
  ShapeFunctionSet psfs(ordr-1); // for pressure
  //neighbor shape functions
   
  //shape function size and total dof
  int vdof=vsfs.size()*2; // two velocity components and total velocity sfs size
  int pdof=psfs.size();
  //get parameter
  DGStokesParameters parameter;
  //get the geometry type of the face
  Dune::GeometryType gtboundary = *ispointer.intersectionSelfLocal().type();
  //specify the quadrature order ?
  int qord=18;
  for(int bq=0;bq<Dune::QuadratureRules<ctype,dim-1>::rule(gtboundary,qord).size();++bq)
	{
	  const Dune::FieldVector<ctype,dim-1>& boundlocal = Dune::QuadratureRules<ctype,dim-1>::rule(gtboundary,qord)[bq].position();
	  Dune:: FieldVector<ctype,dim-1> blocal = *ispointer.intersectionSelfLocal().global(boundlocal);
	  Dune::FieldVector<ctype,dim-1> bglobal = *ispointer.intersectionGlobal().global(boundlocal);
	  double norm_eb=*ispointer.intersectionGlobal().integrationElement(boundlocal);  
	  // calculating the inverse jacobian 
	  InverseJacobianMatrix inv_jac= *epointer.geometry().jacobianInverseTransposed(blocal);
	  // get quadrature weight
	  double quad_wt_bound = Dune::QuadratureRules<ctype,dim-1>::rule(gtboundary,qord)[bq].weight();
	  ctype detjacbound = *ispointer.intersectionGlobal().integrationElement(boundlocal);
	  // get the boundary normal 
	  Dune::FieldVector<ctype,dim> boundnormal = *ispointer.unitOuterNormal(boundlocal);

	  // finding velocity boundary condition
	  //horizontal component 
	  exact_u(bglobal[0],bglobal[1],dirichlet[0]);
	  //vertical component
	  exact_v(bglobal[0],bglobal[1],dirichlet[1]);

	  
	  //================================================//
	  // 
	  // TERM:3
	  //- (\mu \int \nabla u. normal . v)  
	  //================================================//

	  for(int dm=1;dm<=2;++dm)
		{
		
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i; 
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,blocal);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  for (int sd=0; sd<2; sd++)
					temp[sd] = vsfs[j].evaluateDerivative(0,sd,blocal);
				  grad_phi_ej[dm-1] = 0;
				  inv_jac.umv(temp,grad_phi_ej[dm-1]);
				  entry = ( - parameter.mu * ((grad_phi_ej[dm-1]*boundnormal)*phi_ei[dm-1])) * detjacbound*quad_wt_bound;
				  Aee.add(ii,jj,entry);
				}
			}
		}
	  //================================================//				  
	  //TERM:5=  \mu parameter.epsilon \nabla v . normal. u
	  //  TERM:15
	  // rhs entry:  parameter.mu * parameter.epsilon* g * \nabla v * n
	  //================================================//				  
	  for(int dm=1;dm<=2;++dm)
		{
		 
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i; 
			  for (int sd=0; sd<2; sd++)
				temp[sd] = vsfs[i].evaluateDerivative(0,sd,blocal);
			  grad_phi_ei[dm-1] = 0;
			  inv_jac.umv(temp,grad_phi_ei[dm-1]);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  int jj=(dm-1)*vsfs.size()+j;
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,blocal);
				  //TERM:5 \mu parameter.epsilon \nabla v . normal. u		 
				  entry = parameter.mu *(parameter.epsilon*(grad_phi_ei[dm-1]*boundnormal)*phi_ej[dm-1] ) * detjacbound*quad_wt_bound;
				  Aee.add(ii,jj,entry);
				}
			  //------------------------------------
			  //  TERM:15
			  // rhs entry:  parameter.mu * parameter.epsilon* g * \nabla v * n
			  //------------------------------------
			  Be[ii]+= (parameter.epsilon*parameter.mu*(dirichlet[dm-1])*(grad_phi_ei[dm-1]*boundnormal)) * detjacbound * quad_wt_bound;
			}
		}
	  
	  //================================================//
	  //  TERM:7
	  // + \mu parameter.sigma/norm_e . v . u
	  // TERM:16 
	  // rhs entry: mu*parameter.sigma/norm_e * g * v 
	  //================================================//			  
	  for(int dm=1;dm<=2;++dm)
		{

		  for (int i=0;i<vsfs.size();++i) 
			{
		
			  phi_ei[dm-1] =  vsfs[i].evaluateFunction(0,blocal);
			  int ii=(dm-1)*vsfs.size()+i; 
			  for (int j=0;j<vsfs.size();++j) 
				{
			
				  int jj=(dm-1)*vsfs.size()+j;
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,blocal);
				  entry = ((parameter.mu*(parameter.sigma/norm_eb)*phi_ej[dm-1]*phi_ei[dm-1]))* detjacbound*quad_wt_bound;
				  Aee.add(ii,jj,entry);
				}
			  //------------------------------------
			  // TERM:16 
			  // rhs entry: mu*parameter.sigma/norm_e * g * v 
			  //------------------------------------
			  Be[ii]+= (parameter.mu*(parameter.sigma/norm_eb)*(dirichlet[dm-1])*phi_ei[dm-1])* detjacbound * quad_wt_bound;
			
			}
		  
		}

	  //================================================//
	  // TERM:10
	  // 	  \int p v n
	  //================================================//			  
	  for(int dm=1;dm<=2;++dm)
		{
		  for (int i=0;i<vsfs.size();++i) 
			{
			  int ii=(dm-1)*vsfs.size()+i;
			  phi_ei[dm-1] = vsfs[i].evaluateFunction(0,blocal);
			  for (int j=0;j<psfs.size();++j) 
				{
				  psi_ej = psfs[j].evaluateFunction(0,blocal);
				  int jj=vdof+j;
				  entry= (psi_ej*(phi_ei[dm-1]*boundnormal[dm-1]))* detjacbound * quad_wt_bound;
				  Aee.add(ii,jj,entry);
				}
			}
		}
				  
	  //================================================//
	  // \int q . u . n  --> TERM:13
	  // psi_ej * phi_ei * normal
	  //================================================//

	  for(int dm=1;dm<=2;++dm)
		{		

		  for (int i=0;i<psfs.size();++i) 
			{
			  int ii=vdof+i;
			  psi_ei = psfs[i].evaluateFunction(0,blocal);
			  for (int j=0;j<vsfs.size();++j) 
				{
				  phi_ej[dm-1] = vsfs[j].evaluateFunction(0,blocal);
				  int jj=(dm-1)*vsfs.size()+j;
				  entry= (psi_ei*(phi_ej[dm-1]*boundnormal[dm-1]) )* detjacbound * quad_wt_bound;
				  Aee.add(ii,jj,entry);
			 
				}
			}

		}
				  
	  //================================================// 
	  //TERM:17 (rhs)
	  // \int q . g . n
	  //================================================//
	  for (int i=0;i<psfs.size();++i) 
		{
		  int ii=vdof+i;
		  psi_ei = psfs[i].evaluateFunction(0,blocal);
		  Be[ii]+=(dirichlet[0]*boundnormal[0]+dirichlet[1]*boundnormal[1])*psi_ei*detjacbound*quad_wt_bound;
		}
	}
  
}



template<class G,int ordr>
void DGStokes<G,ordr>::assembleStokesSystem()
{
 ShapeFunctionSet vsfs(ordr);; //for  velocity
 ShapeFunctionSet psfs(ordr-1); // for pressure
  
  int vdof=vsfs.size()*2; // two velocity components and total velocity sfs size
  int pdof=psfs.size();
  int ndof=vdof+pdof; // total dofs per element
  int N = ndof*grid.size(level, 0);
  int nz=N;
  DGStokesParameters parameter;
   Dune::SparseRowMatrix<double> AA(N,N,nz);
   Dune::SimpleVector<double> bb(N);
  bb=0.0;
  // loop over all elements
   ElementIterator it = grid.template lbegin<0>(level);
    ElementIterator itend = grid.template lend<0>(level);
 for (; it != itend; ++it)
    {
	  EntityPointer epointer = it;
	  int eid = grid.levelIndexSet(level).index(*epointer);
	  // Dune::GeometryType gt = *epointer.geometry().type();
	// void assembleVolumeTerm(Entity& ep, LocalMatrixBlock& Aee,LocalVectorBlock& Be) const;
// 	void assembleFaceTerm(Entity& ep, LocalMatrixBlock& Aee,LocalVectorBlock& Be) const;
// 	void assembleBoundaryTerm(Entity& ep, LocalMatrixBlock& Aee,LocalVectorBlock& Be)const ;

	  stokessystem.assembleVolumeTerm(*epointer,AA,bb);


	  
	  //modify matrix for introducing pressrure boundary condition
	  
	}
 
	
}// end of assemble


template<class G,int ordr>
void DGStokes<G,ordr>::solveStokesSystem()
{
  std::cout << "Solving Stokes System using superLU solver\n";
}
