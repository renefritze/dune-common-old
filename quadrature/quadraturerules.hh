#ifndef DUNE_QUADRATURERULES_HH
#define DUNE_QUADRATURERULES_HH
#include<iostream>
#include<vector>
#include"dune/common/fvector.hh"
#include"dune/common/exceptions.hh"
#include"dune/common/stdstreams.hh"
#include"dune/grid/common/grid.hh"

namespace Dune {

  /***********************************************************
   * Interface for quadrature points and rules
   ***********************************************************/

  //! quadrature rules for cubes of any dimension based on Gauss quadrature
  template<typename ct, int dim>
  class QuadraturePoint {
  public:
	// compile time parameters
	enum { d=dim };
	typedef ct CoordType;

	//! set up quadrature of given order in d dimensions
	QuadraturePoint (const FieldVector<ct, dim>& x, double w) : local(x)
	{
	  wght = w;
	}

	//! return local coordinates of integration point i 
	const FieldVector<ct, dim>& position () const
	{
	  return local;
	}

	//! return weight associated with integration point i
	double weight () const
	{
	  return wght;
	}
    virtual ~QuadraturePoint(){}
    
  private:
	FieldVector<ct, dim> local;
	double wght;
  };


  //! abstract base class for quadrature rules
  template<typename ct, int dim>
  class QuadratureRule : public std::vector<QuadraturePoint<ct,dim> >
  {
  public:
    QuadratureRule(){}
    // compile time parameters
	enum { d=dim };
	typedef ct CoordType;

	//! return order
	virtual int order () const = 0;

	//! return type of element
	virtual GeometryType type () const = 0;
    virtual ~QuadratureRule(){}
    
  };



  /***********************************************************
   * Gauss quadrature for the n-dimensional cube
   ***********************************************************/

  //! define the one-dimensional Gauss weights
  class GaussPoints
  {
  public:
	enum { MAXP=10 };
	enum { highest_order=19 };

	//! initialize Gauss points on the interval for all orders
	GaussPoints ()
	{
	  int m = 0;
	  O[m] = 0;

	  m = 1;
	  G[m][0] = 0.5;
	  W[m][0] = 1;
	  O[m] = 1;

	  m = 2;
	  G[m][0] = 0.2113248654051871177454256097490212721761991243649365619906988367580112;
	  G[m][1] = 0.7886751345948128822545743902509787278238008756350634380093011632419888;
	  W[m][0] = 0.5;
	  W[m][1] = 0.5;
	  O[m] = 3;

	  m = 3;
	  G[m][0] = 0.112701665379258311482073460021760038916707829470840917341242623388652;
	  G[m][1] = 0.5;
	  G[m][2] = 0.887298334620741688517926539978239961083292170529159082658757376611348;
	  W[m][0] = 0.277777777777777777777777777777777777777777777777777777777777777777778;
	  W[m][1] = 0.444444444444444444444444444444444444444444444444444444444444444444444;
	  W[m][2] = 0.277777777777777777777777777777777777777777777777777777777777777777778;
	  O[m] = 5;

	  m = 4;
	  G[m][0] = 0.069431844202973712388026755553595247452137310185141181192139039546735;
	  G[m][1] = 0.3300094782075718675986671204483776563997120651145428237035230115894900;
	  G[m][2] = 0.6699905217924281324013328795516223436002879348854571762964769884105100;
	  G[m][3] = 0.930568155797026287611973244446404752547862689814858818807860960453265;
	  W[m][0] = 0.173927422568726928686531974610999703617674347916946770246264659759376;
	  W[m][1] = 0.326072577431273071313468025389000296382325652083053229753735340240624;
	  W[m][2] = 0.326072577431273071313468025389000296382325652083053229753735340240624;
	  W[m][3] = 0.173927422568726928686531974610999703617674347916946770246264659759376;
	  O[m] = 7;

	  m = 5;
	  G[m][0] = 0.046910077030668003601186560850303517437174044618734568563118856728115;
	  G[m][1] = 0.2307653449471584544818427896498955975163566965472200218988841864702644;
	  G[m][2] = 0.5;
	  G[m][3] = 0.7692346550528415455181572103501044024836433034527799781011158135297356;
	  G[m][4] = 0.953089922969331996398813439149696482562825955381265431436881143271885;
	  W[m][0] = 0.118463442528094543757132020359958681321630001106207007791413944110859;
	  W[m][1] = 0.239314335249683234020645757417819096456147776671570769986363833666919;
	  W[m][2] = 0.284444444444444444444444444444444444444444444444444444444444444444444;
	  W[m][3] = 0.239314335249683234020645757417819096456147776671570769986363833666919;
	  W[m][4] = 0.118463442528094543757132020359958681321630001106207007791413944110859;
	  O[m] = 9;

	  m = 6;
	  G[m][0] = 0.033765242898423986093849222753002695432617131143855087563725191736693;
	  G[m][1] = 0.1693953067668677431693002024900473264967757178024149645927366470739083;
	  G[m][2] = 0.3806904069584015456847491391596440322906946849299893249093024177128625;
	  G[m][3] = 0.6193095930415984543152508608403559677093053150700106750906975822871375;
	  G[m][4] = 0.8306046932331322568306997975099526735032242821975850354072633529260917;
	  G[m][5] = 0.966234757101576013906150777246997304567382868856144912436274808263307;
	  W[m][0] = 0.085662246189585172520148071086366446763411250742021991199317719899473;
	  W[m][1] = 0.180380786524069303784916756918858055830760946373372741144869620118570;
	  W[m][2] = 0.233956967286345523694935171994775497405827802884605267655812659981957;
	  W[m][3] = 0.233956967286345523694935171994775497405827802884605267655812659981957;
	  W[m][4] = 0.180380786524069303784916756918858055830760946373372741144869620118570;
	  W[m][5] = 0.085662246189585172520148071086366446763411250742021991199317719899473;
	  O[m] = 11;

	  m = 7;
	  G[m][0] = 0.025446043828620737736905157976074368799614531164691108225615448043468;
	  G[m][1] = 0.129234407200302780068067613359605796462926176429304869940022324016285;
	  G[m][2] = 0.2970774243113014165466967939615192683263089929503149368064783741026681;
	  G[m][3] = 0.5;
	  G[m][4] = 0.7029225756886985834533032060384807316736910070496850631935216258973319;
	  G[m][5] = 0.870765592799697219931932386640394203537073823570695130059977675983715;
	  G[m][6] = 0.974553956171379262263094842023925631200385468835308891774384551956532;
	  W[m][0] = 0.064742483084434846635305716339541009164293701129973331988604319362328;
	  W[m][1] = 0.139852695744638333950733885711889791243462532613299382268507016346809;
	  W[m][2] = 0.190915025252559472475184887744487566939182541766931367375541725515353;
	  W[m][3] = 0.208979591836734693877551020408163265306122448979591836734693877551020;
	  W[m][4] = 0.190915025252559472475184887744487566939182541766931367375541725515353;
	  W[m][5] = 0.139852695744638333950733885711889791243462532613299382268507016346809;
	  W[m][6] = 0.064742483084434846635305716339541009164293701129973331988604319362328;
	  O[m] = 13;

	  m = 8;
	  G[m][0] = 0.019855071751231884158219565715263504785882382849273980864180111313788;
	  G[m][1] = 0.101666761293186630204223031762084781581414134192017583964914852480391;
	  G[m][2] = 0.2372337950418355070911304754053768254790178784398035711245714503637726;
	  G[m][3] = 0.4082826787521750975302619288199080096666210935435131088414057631503978;
	  G[m][4] = 0.5917173212478249024697380711800919903333789064564868911585942368496022;
	  G[m][5] = 0.7627662049581644929088695245946231745209821215601964288754285496362274;
	  G[m][6] = 0.898333238706813369795776968237915218418585865807982416035085147519609;
	  G[m][7] = 0.980144928248768115841780434284736495214117617150726019135819888686212;
	  W[m][0] = 0.050614268145188129576265677154981095057697045525842478529501849032370;
	  W[m][1] = 0.111190517226687235272177997213120442215065435025624782362954644646808;
	  W[m][2] = 0.156853322938943643668981100993300656630164499501367468845131972537478;
	  W[m][3] = 0.181341891689180991482575224638597806097073019947165270262411533783343;
	  W[m][4] = 0.181341891689180991482575224638597806097073019947165270262411533783343;
	  W[m][5] = 0.156853322938943643668981100993300656630164499501367468845131972537478;
	  W[m][6] = 0.111190517226687235272177997213120442215065435025624782362954644646808;
	  W[m][7] = 0.050614268145188129576265677154981095057697045525842478529501849032370;
	  O[m] = 15;

	  m = 9;
	  G[m][0] = 0.015919880246186955082211898548163564975297599754037335224988344075460;
	  G[m][1] = 0.081984446336682102850285105965132561727946640937662001947814010180272;
	  G[m][2] = 0.1933142836497048013456489803292629076071396975297176535635935288593663;
	  G[m][3] = 0.3378732882980955354807309926783316957140218696315134555864762615789067;
	  G[m][4] = 0.5;
	  G[m][5] = 0.6621267117019044645192690073216683042859781303684865444135237384210933;
	  G[m][6] = 0.8066857163502951986543510196707370923928603024702823464364064711406337;
	  G[m][7] = 0.918015553663317897149714894034867438272053359062337998052185989819728;
	  G[m][8] = 0.984080119753813044917788101451836435024702400245962664775011655924540;
	  W[m][0] = 0.040637194180787205985946079055261825337830860391205375355553838440343;
	  W[m][1] = 0.090324080347428702029236015621456404757168910866020242249167953235679;
	  W[m][2] = 0.130305348201467731159371434709316424885920102218649975969998501059805;
	  W[m][3] = 0.156173538520001420034315203292221832799377430630952322777005582799572;
	  W[m][4] = 0.165119677500629881582262534643487024439405391786344167296548248929201;
	  W[m][5] = 0.156173538520001420034315203292221832799377430630952322777005582799572;
	  W[m][6] = 0.130305348201467731159371434709316424885920102218649975969998501059805;
	  W[m][7] = 0.090324080347428702029236015621456404757168910866020242249167953235679;
	  W[m][8] = 0.040637194180787205985946079055261825337830860391205375355553838440343;
	  O[m] = 17;

	  m = 10;
	  G[m][0] = 0.013046735741414139961017993957773973285865026653808940384393966651702;
	  G[m][1] = 0.067468316655507744633951655788253475736228492517334773739020134077313;
	  G[m][2] = 0.160295215850487796882836317442563212115352644082595266167591405523721;
	  G[m][3] = 0.2833023029353764046003670284171079188999640811718767517486492434281165;
	  G[m][4] = 0.4255628305091843945575869994351400076912175702896541521460053732420482;
	  G[m][5] = 0.5744371694908156054424130005648599923087824297103458478539946267579518;
	  G[m][6] = 0.7166976970646235953996329715828920811000359188281232482513507565718835;
	  G[m][7] = 0.839704784149512203117163682557436787884647355917404733832408594476279;
	  G[m][8] = 0.932531683344492255366048344211746524263771507482665226260979865922687;
	  G[m][9] = 0.986953264258585860038982006042226026714134973346191059615606033348298;
	  W[m][0] = 0.033335672154344068796784404946665896428932417160079072564347440806706;
	  W[m][1] = 0.074725674575290296572888169828848666201278319834713683917738634376619;
	  W[m][2] = 0.109543181257991021997767467114081596229385935261338544940478271817600;
	  W[m][3] = 0.134633359654998177545613460784734676429879969230441897900281638121077;
	  W[m][4] = 0.147762112357376435086946497325669164710523358513426800677154014877998;
	  W[m][5] = 0.147762112357376435086946497325669164710523358513426800677154014877998;
	  W[m][6] = 0.134633359654998177545613460784734676429879969230441897900281638121077;
	  W[m][7] = 0.109543181257991021997767467114081596229385935261338544940478271817600;
	  W[m][8] = 0.074725674575290296572888169828848666201278319834713683917738634376619;
	  W[m][9] = 0.033335672154344068796784404946665896428932417160079072564347440806706;
	  O[m] = 19;
	}

	double point (int m, int i)
	{
	  return G[m][i];
	}

	double weight (int m, int i)
	{
	  return W[m][i];
	}

	int order (int m)
	{
	  return O[m];
	}

  private:
    double G[MAXP+1][MAXP]; // positions of Gauss points
	double W[MAXP+1][MAXP]; // weights associated with points       
	int    O[MAXP+1];       // order of the rule
  };

  
  /** Singleton holding the Gauss points on the interval */
  struct GaussPointsSingleton {
	static GaussPoints gp;
  };

  // initialize static variable; constructor initializes the array
  GaussPoints GaussPointsSingleton::gp;

  //! A cube quadrature rule for a fixed order is a container of cube quadrature points
  template<typename ct, int dim>
  class CubeQuadratureRule : public QuadratureRule<ct,dim>
  {
  public:
	// compile time parameters
	enum { d=dim };
	enum { highest_order=GaussPoints::highest_order };
	typedef ct CoordType;
	typedef CubeQuadratureRule value_type;

	//! set up quadrature of given order in d dimensions
	CubeQuadratureRule (int p)
	{
	  // find the right Gauss Rule from given order
	  int m=0;
	  for (int i=0; i<=GaussPoints::MAXP; i++)
		if (GaussPointsSingleton::gp.order(i)>=p)
		  {
			m = i;
			break;
		  }
	  if (m==0) DUNE_THROW(NotImplemented, "order not implemented");
	  delivered_order = GaussPointsSingleton::gp.order(m);

	  // fill in all the gauss points
	  n = power(m,dim);
	  for (int i=0; i<n; i++)
		{
		  // compute multidimensional coordinates of Gauss point
		  int x[dim];
		  int z = i;
		  for (int k=0; k<dim; ++k)
			{
			  x[k] = z%m;
			  z = z/m;
			}

		  // compute coordinates and weight
		  double weight = 1.0;
		  FieldVector<ct, dim> local;
		  for (int k=0; k<dim; k++) 
			{
			  local[k] = GaussPointsSingleton::gp.point(m,x[k]);
			  weight *= GaussPointsSingleton::gp.weight(m,x[k]);
			}

		  // put in container
		  push_back(QuadraturePoint<ct,dim>(local,weight));
		}
	}

	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return hexahedron;
	}

	//! appear as your own container
	const CubeQuadratureRule& getelement (GeometryType type, int p)
	{
	  return *this;
	}
    ~CubeQuadratureRule(){}
  private:

	int delivered_order;  // delivered order
	int n;      // number of integration points

	int power (int y, int d)
	{
	  int m=1;
	  for (int i=0; i<d; i++) m *= y;
	  return m;
	}
  };


   /*********************************
   * Quadrature rules for Simplices
   ***********************************/

  //--specialization for simplex DIM==1
  //--it use cube quadrature rule for DIM==1
template<typename ct, int dim>
class SimplexQuadratureRule;

template<typename ct>
class SimplexQuadratureRule<ct,1> : public QuadratureRule<ct,1>
{
public:
  enum{dim=1};
  enum{highest_order=GaussPoints::highest_order};
  typedef ct CoordType;
  typedef SimplexQuadratureRule value_type;
  SimplexQuadratureRule(int p)
  {
    

// find the right Gauss Rule from given order
	  int m=0;
	  for (int i=0; i<=GaussPoints::MAXP; i++)
		if (GaussPointsSingleton::gp.order(i)>=p)
		  {
			m = i;
			break;
		  }
	  if (m==0) DUNE_THROW(NotImplemented, "order not implemented");
	  delivered_order = GaussPointsSingleton::gp.order(m);

	  // fill in all the gauss points
	  n = power(m,dim);
	  for (int i=0; i<n; i++)
		{
		  // compute multidimensional coordinates of Gauss point
		  int x[dim];
		  int z = i;
		  for (int k=0; k<dim; ++k)
			{
			  x[k] = z%m;
			  z = z/m;
			}

		  // compute coordinates and weight
		  double weight = 1.0;
		  FieldVector<ct, dim> local;
		  for (int k=0; k<dim; k++) 
			{
			  local[k] = GaussPointsSingleton::gp.point(m,x[k]);
			  weight *= GaussPointsSingleton::gp.weight(m,x[k]);
			}

		  // put in container
		  push_back(QuadraturePoint<ct,dim>(local,weight));
		}
	}



	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return line;
	}

	//! appear as your own container
	const SimplexQuadratureRule<ct,2>& getelement (GeometryType type, int p)
	{
	  return *this;
	}
   ~SimplexQuadratureRule(){}
  

private:
  int delivered_order;
  int n; // integration points
  int power (int y,int d)
  {
    int m=1;
    for (int i=0;i<d;++i) m*=y;
    return m;
  }
  
};


   /************************************************
   * Quadrature points for Simplices/ triangle
   *************************************************/

  //! 
  template<int dim>
  class SimplexQuadraturePoints;

  template<>
  class SimplexQuadraturePoints<2>
  
  {
  public:
	enum { MAXP=7};
	enum { highest_order=5 };

	//! initialize quadrature points on the interval for all orders
	SimplexQuadraturePoints ()
	{
	  int m = 0;
	  O[m] = 0;
          
	  // polynom degree 1
	  m = 1;
	  G[m][0][0] = 0.3333333333;
	  G[m][0][1] = 0.3333333333;
	  W[m][0] = 1;
	  O[m] = 1;
	  
	   // polynom degree 2
	  // symmetric
	  m = 3;
	  G[m][0][0] = 4.0/6.0;
	  G[m][0][1] = 1.0/6.0;
	  G[m][1][0] = 1.0/6.0;
	  G[m][1][1] = 4.0/6.0;
	  G[m][2][0] = 1.0/6.0;
	  G[m][2][1] = 1.0/6.0;
	  W[m][0] = 1.0/3.0;
	  W[m][1] = 1.0/3.0;
	  W[m][2] = 1.0/3.0;
	  O[m] = 2;

	  // polynom degree 3
	  // symmetric
	  m = 4;
	  G[m][0][0] = 10.0/30.0;
	  G[m][0][1] = 10.0/30.0;
	  G[m][1][0] = 18.0/30.0;
	  G[m][1][1] = 6.0/30.0;
	  G[m][2][0] = 6.0/30.0;
	  G[m][2][1] = 18.0/30.0;
	  G[m][3][0] = 6.0/30.0;
	  G[m][3][1] = 6.0/30.0;

	  W[m][0] = -27.0/48.0;
	  W[m][1] = 25.0/48.0;
	  W[m][2] = 25.0/48.0;
	  W[m][3] = 25.0/48.0;
	  O[m] = 3;
	  // polynomial degree 4
	  // symmetric points
	  m = 6;
	  G[m][0][0] = 0.81684757298045851308085707319560;
	  G[m][0][1] = 0.091576213509770743459571463402202;
	  G[m][1][0] = 0.091576213509770743459571463402202;
	  G[m][1][1] = 0.81684757298045851308085707319560;
	  G[m][2][0] = 0.091576213509770743459571463402202;
	  G[m][2][1] = 0.091576213509770743459571463402202;
	  G[m][3][0] = 0.10810301816807022736334149223390;
	  G[m][3][1] = 0.44594849091596488631832925388305;
	  G[m][4][0] = 0.44594849091596488631832925388305;
	  G[m][4][1] = 0.10810301816807022736334149223390;
	  G[m][5][0] = 0.44594849091596488631832925388305;
	  G[m][5][1] = 0.44594849091596488631832925388305;
	  
	  W[m][0] = 0.10995174365532186763832632490021;
	  W[m][1] = 0.10995174365532186763832632490021;
	  W[m][2] = 0.10995174365532186763832632490021;
	  W[m][3] = 0.22338158967801146569500700843312;
	  W[m][4] = 0.22338158967801146569500700843312;
	  W[m][5] = 0.22338158967801146569500700843312;
	  O[m] = 4;

	  // polynomial degree 5
	  // symmetric points

	  m = 7;
	  G[m][0][0] = 0.333333333333333333333333333333333;
	  G[m][0][1] = 0.333333333333333333333333333333333;
	  G[m][1][0] = 0.79742698535308732239802527616975; 
	  G[m][1][1] = 0.1012865073234563388009873619151;
	  G[m][2][0] = 0.10128650732345633880098736191512;
	  G[m][2][1] = 0.79742698535308732239802527616975;
	  G[m][3][0] = 0.10128650732345633880098736191512;
	  G[m][3][1] = 0.10128650732345633880098736191512;
	  G[m][4][0] = 0.05971587178976982045911758097311;
	  G[m][4][1] = 0.47014206410511508977044120951345;
	  G[m][5][0] = 0.47014206410511508977044120951345;
	  G[m][5][1] = 0.05971587178976982045911758097311;
	  G[m][6][0] = 0.47014206410511508977044120951345;
	  G[m][6][1] = 0.47014206410511508977044120951345;

	  W[m][0] = 0.225;
	  W[m][1] = 0.12593918054482715259568394550018;
	  W[m][2] = 0.12593918054482715259568394550018;
	  W[m][3] = 0.12593918054482715259568394550018;
	  W[m][4] = 0.13239415278850618073764938783315;
	  W[m][5] = 0.13239415278850618073764938783315;
	  W[m][6] = 0.13239415278850618073764938783315;
	  O[m] = 5;
	 
	}

	 FieldVector<double, 2> point(int m, int i)
	{
	  return G[m][i];
	}

	double weight (int m, int i)
	{
	  return W[m][i];
	}

	int order (int m)
	{
	  return O[m];
	}

  private:
  FieldVector<double, 2> G[MAXP+1][MAXP];
    
	double W[MAXP+1][MAXP]; // weights associated with points       
	int    O[MAXP+1];       // order of the rule
  };


/** Singleton holding the Gauss points on the interval */
template<int dim>  
struct SimplexQuadraturePointsSingleton {
	static SimplexQuadraturePoints<dim> sqp;
  };

template<>  
struct SimplexQuadraturePointsSingleton<2> {
	static SimplexQuadraturePoints<2> sqp;
  };

SimplexQuadraturePoints<2> SimplexQuadraturePointsSingleton<2>::sqp;


template<typename ct, int dim>
class SimplexQuadratureRule;

template<typename ct>
class SimplexQuadratureRule<ct,2> : public QuadratureRule<ct,2>
{
public:
  enum{d=2};
  enum{highest_order=SimplexQuadraturePoints<2>::highest_order};
  typedef ct CoordType;
  typedef SimplexQuadratureRule value_type;
  SimplexQuadratureRule(int p)
  {
    switch(p)
      {
      case 0: // to be verified
	m=1; // to be verified
	break;
      case 1:
	m=1;
	break;
      case 2:
	m=3;
	break;
      case 3:
	m=4;
	break;
      case 4:
	m=6;
	break;
      case 5:
	m=7;
	break;
      default: m=7;
      }
  delivered_order = SimplexQuadraturePointsSingleton<2>::sqp.order(m);
  FieldVector<ct, d> local;
  double weight;
  for(int i=0;i<m;++i)
    {
      for(int k=0;k<d;++k)
	local[k]=SimplexQuadraturePointsSingleton<2>::sqp.point(m,i)[k];
      weight=SimplexQuadraturePointsSingleton<2>::sqp.weight(m,i);
// put in container
  push_back(QuadraturePoint<ct,d>(local,weight));	
      
    }
  
  }

	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return triangle;
	}

	//! appear as your own container
	const SimplexQuadratureRule<ct,2>& getelement (GeometryType type, int p)
	{
	  return *this;
	}
   ~SimplexQuadratureRule(){}
  

private:
  int delivered_order, m;
};



/***********************************************************
 * Gauss quadrature for Simplices/ tetrahedron
 ***********************************************************/
template<int dim>
  class SimplexQuadraturePoints;

  template<>
  class SimplexQuadraturePoints<3>
  //! 
  {
  public:
	enum { MAXP=8};
	enum { highest_order=3 };

	//! initialize quadrature points on the interval for all orders
	SimplexQuadraturePoints()
	{
	  int m = 0;
	  O[m] = 0;
          
	  // polynom degree 1
	  m = 1;
	  G[m][0][0] = 0.25;
	  G[m][0][1] = 0.25;
	  G[m][0][2] = 0.25;

	  W[m][0] = 1;
	  O[m] = 1;
	  
	   // polynom degree 2
	  // symmetric
	  m = 4;
	  G[m][0][0] = 0.58541020;
	  G[m][0][1] = 0.13819660;
	  G[m][0][2] = 0.13819660;
	  G[m][1][0] = 0.13819660;
	  G[m][1][1] = 0.58541020;
	  G[m][1][2] = 0.13819660; 
	  G[m][2][0] = 0.13819660;  
	  G[m][2][1] = 0.13819660; 
	  G[m][2][2] = 0.58541020;
	  G[m][3][0] = 0.13819660; 
	  G[m][3][1] = 0.13819660; 
	  G[m][3][2] = 0.13819660; 
	  
	  W[m][0] = 1.0/4.0;
	  W[m][1] = 1.0/4.0;
	  W[m][2] = 1.0/4.0;
	  W[m][3] = 1.0/4.0;
	  O[m] = 2;

	  // polynom degree 3
	  // symmetric
	  m = 8;
	  G[m][0][0] = 0.0;
	  G[m][0][1] = 0.0;
	  G[m][0][2] = 0.0;
	  G[m][1][0] = 1.0;
	  G[m][1][1] = 0.0;
	  G[m][1][2] = 0.0;
	  G[m][2][0] = 0.0;
	  G[m][2][1] = 1.0;
	  G[m][2][2] = 0.0;
	  G[m][3][0] = 0.0;
	  G[m][3][1] = 0.0;
	  G[m][3][2] = 1.0;
	  G[m][4][0] = 0.333333333333; 
	  G[m][4][1] = 0.333333333333;
	  G[m][4][2] = 0.0;
	  G[m][5][0] = 0.333333333333;
	  G[m][5][1] = 0.0;
	  G[m][5][2] = 0.333333333333;
	  G[m][6][0] = 0.0;
	  G[m][6][1] = 0.333333333333;
	  G[m][6][2] = 0.333333333333;
	  G[m][7][0] = 0.333333333333;
	  G[m][7][1] = 0.333333333333;
	  G[m][7][2] = 0.333333333333;
	  W[m][0] = 0.025;
	  W[m][1] = 0.025;
	  W[m][2] = 0.025;
	  W[m][3] = 0.025;
	  W[m][4] = 0.225;
	  W[m][5] = 0.225;
	  W[m][6] = 0.225;
	  W[m][7] = 0.225;
	  O[m] = 3;
	  // polynomial degree 4
	  // symmetric points // yet to add
	 
	  // polynomial degree 5
	  // symmetric points // yet to add

	 
	}

	 FieldVector<double, 3> point(int m, int i)
	{
	  return G[m][i];
	}

	double weight (int m, int i)
	{
	  return W[m][i];
	}

	int order (int m)
	{
	  return O[m];
	}

  private:
  FieldVector<double, 3> G[MAXP+1][MAXP];
    //double G[MAXP+1][MAXP]; // positions of Gauss points
	double W[MAXP+1][MAXP]; // weights associated with points       
	int    O[MAXP+1];       // order of the rule
  };


/** Singleton holding the SimplexQuadrature points dim==3 */

template<>  
struct SimplexQuadraturePointsSingleton<3> {
	static SimplexQuadraturePoints<3> sqp;
  };

SimplexQuadraturePoints<3> SimplexQuadraturePointsSingleton<3>::sqp;



template<typename ct, int dim>
class SimplexQuadratureRule;

template<typename ct>
class SimplexQuadratureRule<ct,3> : public QuadratureRule<ct,3>
{

public:
  enum{d=3};
  enum{highest_order=SimplexQuadraturePoints<3>::highest_order};
  typedef ct CoordType;
  typedef SimplexQuadratureRule<ct,3> value_type;
  SimplexQuadratureRule(int p)
  {
    switch(p)
      {
      case 0: // to be verified
	m=1; // to be verified
	break;
      case 1:
	m=1;
	break;
      case 2:
	m=4;
	break;
      case 3:
	m=8;
	break;
      default: m=8;
      }
  delivered_order = SimplexQuadraturePointsSingleton<3>::sqp.order(m);
  FieldVector<ct, d> local;
  double weight;
  for(int i=0;i<m;++i)
    { 
      for(int k=0;k<d;++k)
     local[k]=SimplexQuadraturePointsSingleton<3>::sqp.point(m,i)[k];
      weight=SimplexQuadraturePointsSingleton<3>::sqp.weight(m,i);
// put in container
  push_back(QuadraturePoint<ct,d>(local,weight));	
      
    }
  
  }

	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return tetrahedron;
	}

	//! appear as your own container
	const SimplexQuadratureRule<ct,d>& getelement (GeometryType type, int p)
	{
	  return *this;
	}

 ~SimplexQuadratureRule(){}
private:
  int delivered_order, m;
};



/***********************************
 * quadrature for Prism
 **********************************/

  //! 
template<int dim>
class PrismQuadraturePoints;

template<>
  class PrismQuadraturePoints<3>
  {
  public:
	enum { MAXP=6};
	enum { highest_order=2 };

	//! initialize quadrature points on the interval for all orders
	PrismQuadraturePoints ()
	{
	  int m = 0;
	  O[m] = 0;
          
	  // polynom degree 0  ???
	  m = 6;
	  G[m][0][0] = 0.0;
	  G[m][0][1] = 0.0;
	  G[m][0][2] = 0.0;

	  G[m][1][0] = 1.0;
	  G[m][1][1] = 0.0;
	  G[m][1][2] = 0.0;

	  G[m][2][0] = 0.0;
	  G[m][2][1] = 1.0;
	  G[m][2][2] = 0.0;

	  G[m][3][0] = 0.0;
	  G[m][3][1] = 0.0;
	  G[m][3][2] = 1.0;
          
	  G[m][4][0] = 1.0;
	  G[m][4][1] = 0.0;
	  G[m][4][2] = 1.0;

	  G[m][5][0] = 0.0;
	  G[m][5][1] = 0.1;
	  G[m][5][2] = 1.0;

	  W[m][0] = 0.16666666666666666;
	  W[m][1] = 0.16666666666666666;
	  W[m][2] = 0.16666666666666666;
	  W[m][3] = 0.16666666666666666;
	  W[m][4] = 0.16666666666666666;
	  W[m][5] = 0.16666666666666666;
	  
	  O[m] = 0;// verify ????????
	  

// polynom degree 2  ???
	  m = 6;
	  G[m][0][0] =0.66666666666666666 ;
	  G[m][0][1] =0.16666666666666666 ;
	  G[m][0][2] =0.211324865405187 ;

	  G[m][1][0] = 0.16666666666666666;
	  G[m][1][1] =0.66666666666666666 ;
	  G[m][1][2] = 0.211324865405187;

	  G[m][2][0] = 0.16666666666666666;
	  G[m][2][1] = 0.16666666666666666;
	  G[m][2][2] = 0.211324865405187;

	  G[m][3][0] = 0.66666666666666666;
	  G[m][3][1] = 0.16666666666666666;
	  G[m][3][2] = 0.788675134594813;
          
	  G[m][4][0] = 0.16666666666666666;
	  G[m][4][1] = 0.66666666666666666;
	  G[m][4][2] = 0.788675134594813;

	  G[m][5][0] = 0.16666666666666666;
	  G[m][5][1] = 0.16666666666666666;
	  G[m][5][2] = 0.788675134594813;

	  W[m][0] = 0.16666666666666666;
	  W[m][1] = 0.16666666666666666;
	  W[m][2] = 0.16666666666666666;
	  W[m][3] = 0.16666666666666666;
	  W[m][4] = 0.16666666666666666;
	  W[m][5] = 0.16666666666666666;
	  
	  O[m] = 2;// verify ????????
	 
	}

	 FieldVector<double, 3> point(int m, int i)
	{
	  return G[m][i];
	}

	double weight (int m, int i)
	{
	  return W[m][i];
	}

	int order (int m)
	{
	  return O[m];
	}

  private:
    FieldVector<double, 3> G[MAXP+1][MAXP];//positions
    
	double W[MAXP+1][MAXP]; // weights associated with points       
	int    O[MAXP+1];       // order of the rule
  };


/** Singleton holding the Prism Quadrature points  */
template<int dim>  
struct PrismQuadraturePointsSingleton {
	static PrismQuadraturePoints<3> prqp;
};
template<>  
struct PrismQuadraturePointsSingleton<3> {
	static PrismQuadraturePoints<3> prqp;
  };

PrismQuadraturePoints<3> PrismQuadraturePointsSingleton<3>::prqp;

template<typename ct, int dim>
class PrismQuadratureRule;

template<typename ct>
class PrismQuadratureRule<ct,3> : public QuadratureRule<ct,3>
{
public:
  enum{d=3};
  enum{highest_order=PrismQuadraturePoints<3>::highest_order};
  typedef ct CoordType;
  typedef PrismQuadratureRule<ct,3> value_type;
  PrismQuadratureRule(int p)
  {
    switch(p)
      {
      case 2:
	m=6;
	break;
            default: m=6;
      }
  delivered_order = PrismQuadraturePointsSingleton<3>::prqp.order(m);
  FieldVector<ct, d> local;
  double weight;
  for(int i=0;i<m;++i)
    { 
      for(int k=0;k<d;++k)
     local[k]=PrismQuadraturePointsSingleton<3>::prqp.point(m,i)[k];
      weight=PrismQuadraturePointsSingleton<3>::prqp.weight(m,i);
// put in container
  push_back(QuadraturePoint<ct,d>(local,weight));	
      
    }
  
  }

	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return prism;
	}

	//! appear as your own container
	const PrismQuadratureRule<ct,3>& getelement (GeometryType type, int p)
	{
	  return *this;
	}
 ~PrismQuadratureRule(){}

private:
  int delivered_order, m;
};

/***********************************************************
 *   quadrature for Pyramid
 ***********************************************************/

  //! 
template<int dim>
class PyramidQuadraturePoints;

template<>
class PyramidQuadraturePoints<3>
  {
  public:
	enum { MAXP=8};
	enum { highest_order=2 };

	//! initialize quadrature points on the interval for all orders
	PyramidQuadraturePoints()
	{
	  int m = 0;
	  O[m] = 0;
         

// polynom degree 2  ???
	  m = 8;
	  G[m][0][0] =0.58541020;
	  G[m][0][1] =0.72819660;
	  G[m][0][2] =0.13819660;

	  G[m][1][0] =0.13819660;
	  G[m][1][1] =0.72819660;
	  G[m][1][2] =0.13819660;

	  G[m][2][0] =0.13819660;
	  G[m][2][1] =0.27630920;
	  G[m][2][2] =0.58541020;

	  G[m][3][0] =0.13819660;
	  G[m][3][1] =0.27630920;
	  G[m][3][2] =0.13819660;
          
	  G[m][4][0] =0.72819660;
	  G[m][4][1] =0.13819660;
	  G[m][4][2] =0.13819660;

	  G[m][5][0] =0.72819660;
	  G[m][5][1] =0.58541020;
	  G[m][5][2] =0.13819660;

	  G[m][6][0] =0.27630920;
	  G[m][6][1] =0.13819660;
	  G[m][6][2] =0.58541020;

	  G[m][7][0] =0.27630920;
	  G[m][7][1] =0.13819660;
	  G[m][7][2] =0.13819660;

	  W[m][0] = 0.125;
	  W[m][1] = 0.125;
	  W[m][2] = 0.125;
	  W[m][3] = 0.125;
	  W[m][4] = 0.125;
	  W[m][5] = 0.125;
	  W[m][6] = 0.125;
	  W[m][7] = 0.125;
	  
	  O[m] = 2;// verify ????????
	 
	}

	 FieldVector<double, 3> point(int m, int i)
	{
	  return G[m][i];
	}

	double weight (int m, int i)
	{
	  return W[m][i];
	}

	int order (int m)
	{
	  return O[m];
	}

  private:
  FieldVector<double, 3> G[MAXP+1][MAXP];
    //double G[MAXP+1][MAXP]; // positions of Gauss points
	double W[MAXP+1][MAXP]; // weights associated with points       
	int    O[MAXP+1];       // order of the rule
  };


/** Singleton holding the Quadrature  points  */

template<int dim>
  struct PyramidQuadraturePointsSingleton {
	static PyramidQuadraturePoints<3> pyqp;
  };
template<>
  struct PyramidQuadraturePointsSingleton<3> {
	static PyramidQuadraturePoints<3> pyqp;
  };

PyramidQuadraturePoints<3> PyramidQuadraturePointsSingleton<3>::pyqp;

template<typename ct, int dim>
class PyramidQuadratureRule; 

template<typename ct>
class PyramidQuadratureRule<ct,3> : public QuadratureRule<ct,3>
{
public:
  enum{d=3};
  enum{highest_order=PyramidQuadraturePoints<3>::highest_order};
  typedef ct CoordType;
  typedef PyramidQuadratureRule<ct,3> value_type;
  PyramidQuadratureRule(int p)
  {
    switch(p)
      {
   default: m=8;
      }
  
  delivered_order = PyramidQuadraturePointsSingleton<3>::pyqp.order(m);
  FieldVector<ct, d> local;
  double weight;
  for(int i=0;i<m;++i)
    { 
      for(int k=0;k<d;++k)
     local[k]=PyramidQuadraturePointsSingleton<3>::pyqp.point(m,i)[k];
      weight=PyramidQuadraturePointsSingleton<3>::pyqp.weight(m,i);
// put in container
  push_back(QuadraturePoint<ct,d>(local,weight));	
      
    }
  
  }

	//! return order
	int order () const
	{
	  return delivered_order;
	}

	//! return type of element
	GeometryType type () const
	{
	  return pyramid;
	}

	//! appear as your own container
	const PyramidQuadratureRule<ct,3>& getelement (GeometryType type, int p)
	{
	  return *this;
	}
  ~PyramidQuadratureRule(){}

private:
  int delivered_order, m;
};



  /***********************************************************
   * The general container and the singleton
   ***********************************************************/

  //! A container for all quadrature rules
  template<typename ct, int dim>
  class QuadratureRuleContainer
  {
  public:
    typedef QuadratureRule<ct,dim> value_type;

	//! make rules for all element types up to given order
	QuadratureRuleContainer (int pmax)
	{
	  // initialize index counter for rules in the array
	  int index=0;
	  int cubeindex=0;
	  int simpindex=0;
	  
	  ////////////////////////
	  // the cube rules
	  ////////////////////////

	  // allocate all rules up to requested order	  
	  for (int m=1; m<=GaussPoints::MAXP; m++)
		{
		  int p = GaussPointsSingleton::gp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new CubeQuadratureRule<ct,dim>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  cube_order_to_index[i] = index;
			  else
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  if (rules[cube_order_to_index[i]]->order()<i)
					cube_order_to_index[i] = index;
			 
			  ++index;
			  cubeindex=index;
			}
		  else break;
		}
	  ////////////
	  // simplex rule
	  /////////
	  
	  for (int m=1; m<=SimplexQuadraturePoints<dim>::MAXP; m++)
	    {
	      int p = SimplexQuadraturePointsSingleton<dim>::sqp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new SimplexQuadratureRule<ct,dim>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=SimplexQuadraturePoints<dim>::highest_order; i++) 
				  simplex_order_to_index[i] = index;
			  else
				for (int i=0; i<=SimplexQuadraturePoints<dim>::highest_order; i++) 
				  if (rules[simplex_order_to_index[i]]->order()<i)
					simplex_order_to_index[i] = index;
			 
			  ++index;
			  simpindex=index;
			}
		  else break;
		}
	 
	
	  // check if order can be achieved
	   
	  cube_maxorder = rules[cubeindex-1]->order();
	  if (cube_maxorder<pmax)
              dverb << "Warning: Quadrature rule order " << pmax
                    << " requested for cubes but only " << cube_maxorder << " available" << std::endl;

	  simplex_maxorder = rules[simpindex-1]->order();
	  if(simplex_maxorder<pmax)
              dverb << "Warning: Quadrature rule order " << pmax
                    << " requested for simplex-" << dim <<"D"<<" but only " << simplex_maxorder << " available" << std::endl;
 
	}


	//! select the appropriate rule
	const QuadratureRule<ct,dim>& operator() (GeometryType type, int p)
	{
	  if ( (type==cube) || (type==line) || (type==quadrilateral) ||
		   (type==hexahedron) )
		{
		  
		  if (p>=1 && p<=cube_maxorder)
			return *(rules[cube_order_to_index[p]]);
		}

          else  if( (type==simplex) || (type==triangle) || (type==tetrahedron))
	    {
	      if(dim>=2)
		{
		  if(p>=1 && p<=simplex_maxorder)
		    return *(rules[simplex_order_to_index[p]]);
		}
	      else
		{
		  if (p>=1 && p<=cube_maxorder)
		    return *(rules[cube_order_to_index[p]]);
		}
	    }
	  
	  DUNE_THROW(NotImplemented, "order not available");
	}

	//! ConstIterator class for sequential access
	class const_iterator 
	{
	public:
	  //! construct from stl iterator
	  const_iterator (const typename std::vector<QuadratureRule<ct,dim>*>::const_iterator& _i) : i(_i)
	  {       }

	  //! prefix increment
	  const_iterator& operator++()
	  {
        ++i;
		return *this;
	  }
          
	  //! prefix decrement
	  const_iterator& operator--()
	  {
        --i;
		return *this;
	  }
          
	  //! equality
	  bool operator== (const const_iterator& it) const
	  {
        return i==it.i;
	  }

	  //! inequality
	  bool operator!= (const const_iterator& it) const
	  {
        return i!=it.i;
	  }

	  //! dereferencing
	  const QuadratureRule<ct,dim>& operator* () const
	  {
        return **i;
	  }

	  //! arrow
	  const QuadratureRule<ct,dim>* operator-> () const
	  {
        return *i;
	  }

	private:
	  typename std::vector<QuadratureRule<ct,dim>*>::const_iterator i;
	};

	const_iterator begin ()
	{
	  return const_iterator(rules.begin());
	}	

	const_iterator end ()
	{
	  return const_iterator(rules.end());
	}	

  private:
	// the vector of all rules
	std::vector<QuadratureRule<ct,dim>*> rules;

	// mapping for cube rules
    int cube_maxorder;
    int simplex_maxorder;
    int cube_order_to_index[GaussPoints::highest_order+1];
    int simplex_order_to_index[SimplexQuadraturePoints<dim>::highest_order+1];
    

  };

/*-----------------------------------
  Specialization for 1D simplex
  if instantiated 1D simplex, 
  it use ID cube quadrature rule 
   * General container and singletons
   --------------------------------*/

  //! A container for all quadrature rules
  template<typename ct>
  class QuadratureRuleContainer<ct,1>
  {
    enum { dim=1 };
  public:
    typedef QuadratureRule<ct,dim> value_type;

	//! make rules for all element types up to given order
	QuadratureRuleContainer (int pmax)
	{
	  // initialize index counter for rules in the array
	  int index=0;
	  int cubeindex=0;
	  
	  ////////////////////////
	  // the cube rules
	  ////////////////////////

	  // allocate all rules up to requested order	  
	  for (int m=1; m<=GaussPoints::MAXP; m++)
		{
		  int p = GaussPointsSingleton::gp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new SimplexQuadratureRule<ct,dim>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  cube_order_to_index[i] = index;
			  else
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  if (rules[cube_order_to_index[i]]->order()<i)
					cube_order_to_index[i] = index;
			 
			  ++index;
			  cubeindex=index;
			}
		  else break;
		}

	  // check if order can be achieved
	   
	  cube_maxorder = rules[cubeindex-1]->order();
	  if (cube_maxorder<pmax)
              dverb << "Warning: Quadrature rule order " << pmax
                    << " requested for cubes but only " << cube_maxorder << " available" << std::endl;

        }


	//! select the appropriate rule
	const QuadratureRule<ct,dim>& operator() (GeometryType type, int p)
	{
	  if ( (type==cube) || (type==line) ||(type==simplex) )
		{
		  
		  if (p>=1 && p<=cube_maxorder)
			return *(rules[cube_order_to_index[p]]);
		}     
	    
	 
	  DUNE_THROW(NotImplemented, "order not available");
	}

	//! ConstIterator class for sequential access
	class const_iterator 
	{
	public:
	  //! construct from stl iterator
	  const_iterator (const typename std::vector<QuadratureRule<ct,dim>*>::const_iterator& _i) : i(_i)
	  {       }

	  //! prefix increment
	  const_iterator& operator++()
	  {
        ++i;
		return *this;
	  }
          
	  //! prefix decrement
	  const_iterator& operator--()
	  {
        --i;
		return *this;
	  }
          
	  //! equality
	  bool operator== (const const_iterator& it) const
	  {
        return i==it.i;
	  }

	  //! inequality
	  bool operator!= (const const_iterator& it) const
	  {
        return i!=it.i;
	  }

	  //! dereferencing
	  const QuadratureRule<ct,dim>& operator* () const
	  {
        return **i;
	  }

	  //! arrow
	  const QuadratureRule<ct,dim>* operator-> () const
	  {
        return *i;
	  }

	private:
	  typename std::vector<QuadratureRule<ct,dim>*>::const_iterator i;
	};

	const_iterator begin ()
	{
	  return const_iterator(rules.begin());
	}	

	const_iterator end ()
	{
	  return const_iterator(rules.end());
	}	

  private:
	// the vector of all rules
	std::vector<QuadratureRule<ct,dim>*> rules;

	// mapping for cube rules
    int cube_maxorder;
    int cube_order_to_index[GaussPoints::highest_order+1];
  };


  /*-------------------------------------
    specialization for 3D-pyramids/prisms 
     The general container and the singleton 
      --------------------------------------*/

  //! A container for all quadrature rules
  template<typename ct>
  class QuadratureRuleContainer<ct,3>
  {
    enum { dim=3 };
  public:
    typedef QuadratureRule<ct,3> value_type;

	//! make rules for all element types up to given order
	QuadratureRuleContainer (int pmax)
	{
	  // initialize index counter for rules in the array
	  int index=0;
	  int cubeindex=0;
	  int simpindex=0;
	  int prisindex=0;
	  int pyrindex=0;
	  ////////////////////////
	  // the cube rules
	  ////////////////////////

	  // allocate all rules up to requested order	  
	  for (int m=1; m<=GaussPoints::MAXP; m++)
		{
		  int p = GaussPointsSingleton::gp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new CubeQuadratureRule<ct,dim>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  cube_order_to_index[i] = index;
			  else
				for (int i=0; i<=GaussPoints::highest_order; i++) 
				  if (rules[cube_order_to_index[i]]->order()<i)
					cube_order_to_index[i] = index;
			 
			  ++index;
			  cubeindex=index;
			}
		  else break;
		}
	  ////////////
	  // simplex rule
	  /////////
	  
	  for (int m=1; m<=SimplexQuadraturePoints<dim>::MAXP; m++)
	    {
	      int p = SimplexQuadraturePointsSingleton<dim>::sqp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new SimplexQuadratureRule<ct,dim>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=SimplexQuadraturePoints<dim>::highest_order; i++) 
				  simplex_order_to_index[i] = index;
			  else
				for (int i=0; i<=SimplexQuadraturePoints<dim>::highest_order; i++) 
				  if (rules[simplex_order_to_index[i]]->order()<i)
					simplex_order_to_index[i] = index;
			 
			  ++index;
			  simpindex=index;
			}
		  else break;
		}
	 
	  ////////////
	  // prism rule
	  /////////
	  
	  for (int m=1; m<=PrismQuadraturePoints<3>::MAXP; m++)
		{
		  int p = PrismQuadraturePointsSingleton<3>::prqp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new PrismQuadratureRule<ct,3>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=PrismQuadraturePoints<3>::highest_order; i++) 
				  prism_order_to_index[i] = index;
			  else
				for (int i=0; i<=PrismQuadraturePoints<3>::highest_order; i++) 
				  if (rules[prism_order_to_index[i]]->order()<i)
					prism_order_to_index[i] = index;
			 
			  ++index;
			  prisindex=index;
			}
		  else break;
		}

	  ////////////
	  // pyramid rule
	  /////////
	  
	  for (int m=1; m<=PyramidQuadraturePoints<3>::MAXP; m++)
		{
		  int p = PyramidQuadraturePointsSingleton<3>::pyqp.order(m); // order of rule with m points 
		  if (p<=pmax)
			{
			  QuadratureRule<ct,dim>* pointer = new PyramidQuadratureRule<ct,3>(p);
			  rules.push_back(pointer);
			  if (m==1)
				for (int i=0; i<=PyramidQuadraturePoints<3>::highest_order; i++) 
				  pyramid_order_to_index[i] = index;
			  else
				for (int i=0; i<=PyramidQuadraturePoints<3>::highest_order; i++) 
				  if (rules[pyramid_order_to_index[i]]->order()<i)
					pyramid_order_to_index[i] = index;
			 
			  ++index;
			  pyrindex=index;
			}
		  else break;
		}


	  // check if order can be achieved
	   
	  cube_maxorder = rules[cubeindex-1]->order();
	  if (cube_maxorder<pmax)
              dverb << "Warning: Quadrature rule order " << pmax
                    << " requested for cubes but only " << cube_maxorder << " available" << std::endl;

	  simplex_maxorder = rules[simpindex-1]->order();
	  if(simplex_maxorder<pmax)
              dverb << "Warning: Quadrature rule order " << pmax
                    << " requested for simplex-" << (int)dim <<"D"<<" but only " << simplex_maxorder << " available" << std::endl;
 
	prism_maxorder = rules[prisindex-1]->order();
	 if(prism_maxorder<pmax)
             dverb << "Warning: Quadrature rule order " << pmax
                   << " requested for prism" <<" but only " << prism_maxorder << " available" << std::endl;
	pyramid_maxorder = rules[pyrindex-1]->order();	

	if(pyramid_maxorder<pmax)
	    dverb << "Warning: Quadrature rule order " << pmax
                  << " requested for pyramid" <<" but only " << pyramid_maxorder << " available" << std::endl;
	}


	//! select the appropriate rule
	const QuadratureRule<ct,dim>& operator() (GeometryType type, int p)
	{
	  if ( (type==cube) || (type==line) || (type==quadrilateral) ||
		   (type==hexahedron) )
		{
		  
		  if (p>=1 && p<=cube_maxorder)
			return *(rules[cube_order_to_index[p]]);
		}

          else  if( (type==simplex) || (type==tetrahedron))
	    {
	      
		{
		  if(p>=1 && p<=simplex_maxorder)
		    return *(rules[simplex_order_to_index[p]]);
		}
	      
	    }
	  else if (type==prism)
	    {
	      if(p>=1 && p<=prism_maxorder)
		return *(rules[prism_order_to_index[p]]);
	    }
	  else if(type==pyramid)
	    {
	      if(p>=1 && p<=pyramid_maxorder)
		return *(rules[pyramid_order_to_index[p]]);
	    }

	  DUNE_THROW(NotImplemented, "order not available");
	}

	//! ConstIterator class for sequential access
	class const_iterator 
	{
	public:
	  //! construct from stl iterator
	  const_iterator (const typename std::vector<QuadratureRule<ct,dim>*>::const_iterator& _i) : i(_i)
	  {       }

	  //! prefix increment
	  const_iterator& operator++()
	  {
        ++i;
		return *this;
	  }
          
	  //! prefix decrement
	  const_iterator& operator--()
	  {
        --i;
		return *this;
	  }
          
	  //! equality
	  bool operator== (const const_iterator& it) const
	  {
        return i==it.i;
	  }

	  //! inequality
	  bool operator!= (const const_iterator& it) const
	  {
        return i!=it.i;
	  }

	  //! dereferencing
	  const QuadratureRule<ct,dim>& operator* () const
	  {
        return **i;
	  }

	  //! arrow
	  const QuadratureRule<ct,dim>* operator-> () const
	  {
        return *i;
	  }

	private:
	  typename std::vector<QuadratureRule<ct,dim>*>::const_iterator i;
	};

	const_iterator begin ()
	{
	  return const_iterator(rules.begin());
	}	

	const_iterator end ()
	{
	  return const_iterator(rules.end());
	}	

  private:
	// the vector of all rules
	std::vector<QuadratureRule<ct,dim>*> rules;

	// mapping for cube rules
    int cube_maxorder;
   int simplex_maxorder;
    int prism_maxorder;
    int pyramid_maxorder;
    
    int cube_order_to_index[GaussPoints::highest_order+1];
    int simplex_order_to_index[SimplexQuadraturePoints<dim>::highest_order+1];
    int prism_order_to_index[PrismQuadraturePoints<3>::highest_order+1];
    int pyramid_order_to_index[PyramidQuadraturePoints<3>::highest_order+1];

  };




  // singleton holding a quadrature rule container
  template<typename ctype, int dim>
  struct QuadratureRules {
	static QuadratureRuleContainer<ctype,dim> rule;
  };

  template<typename ctype, int dim>
  QuadratureRuleContainer<ctype,dim> QuadratureRules<ctype,dim>::rule(19);



} // end namespace

#endif
