#ifndef __GAUSSQUADRATUREIMP_CC__
#define __GAUSSQUADRATUREIMP_CC__

#include <new>
#include <iostream>
#include <dune/common/fvector.hh>

namespace Dune {

template<class Domain, class RangeField, int dim, int order>
inline GaussQuadrature<Domain,RangeField,dim,order>::GaussQuadrature ()
{
  // could be done better but so what 
        double G[20];
        double W[20];

        if (order<=1)
        {
                //m = 1;
                G[0] = 0.5;
                W[0] = 1;
        }
        else if (order<=3)
        {
                //m = 2;
                G[0] = 0.2113248654051871177454256097490212721761991243649365619906988367580112;
                G[1] = 0.7886751345948128822545743902509787278238008756350634380093011632419888;
                W[0] = 0.5;
                W[1] = 0.5;
        }
        else if (order<=5)
        {
                //m = 3;
                G[0] = 0.112701665379258311482073460021760038916707829470840917341242623388652;
                G[1] = 0.5;
                G[2] = 0.887298334620741688517926539978239961083292170529159082658757376611348;
                W[0] = 0.277777777777777777777777777777777777777777777777777777777777777777778;
                W[1] = 0.444444444444444444444444444444444444444444444444444444444444444444444;
                W[2] = 0.277777777777777777777777777777777777777777777777777777777777777777778;
        }
        else if (order<=7)
        {
          //m = 4;
                G[0] = 0.069431844202973712388026755553595247452137310185141181192139039546735;
                G[1] = 0.3300094782075718675986671204483776563997120651145428237035230115894900;
                G[2] = 0.6699905217924281324013328795516223436002879348854571762964769884105100;
                G[3] = 0.930568155797026287611973244446404752547862689814858818807860960453265;
                W[0] = 0.173927422568726928686531974610999703617674347916946770246264659759376;
                W[1] = 0.326072577431273071313468025389000296382325652083053229753735340240624;
                W[2] = 0.326072577431273071313468025389000296382325652083053229753735340240624;
                W[3] = 0.173927422568726928686531974610999703617674347916946770246264659759376;
        }
        else if (order<=9)
        {
                //m = 5;
                G[0] = 0.046910077030668003601186560850303517437174044618734568563118856728115;
                G[1] = 0.2307653449471584544818427896498955975163566965472200218988841864702644;
                G[2] = 0.5;
                G[3] = 0.7692346550528415455181572103501044024836433034527799781011158135297356;
                G[4] = 0.953089922969331996398813439149696482562825955381265431436881143271885;
                W[0] = 0.118463442528094543757132020359958681321630001106207007791413944110859;
                W[1] = 0.239314335249683234020645757417819096456147776671570769986363833666919;
                W[2] = 0.284444444444444444444444444444444444444444444444444444444444444444444;
                W[3] = 0.239314335249683234020645757417819096456147776671570769986363833666919;
                W[4] = 0.118463442528094543757132020359958681321630001106207007791413944110859;
        }
        else if (order<=11)
        {
                //m = 6;
                G[0] = 0.033765242898423986093849222753002695432617131143855087563725191736693;
                G[1] = 0.1693953067668677431693002024900473264967757178024149645927366470739083;
                G[2] = 0.3806904069584015456847491391596440322906946849299893249093024177128625;
                G[3] = 0.6193095930415984543152508608403559677093053150700106750906975822871375;
                G[4] = 0.8306046932331322568306997975099526735032242821975850354072633529260917;
                G[5] = 0.966234757101576013906150777246997304567382868856144912436274808263307;
                W[0] = 0.085662246189585172520148071086366446763411250742021991199317719899473;
                W[1] = 0.180380786524069303784916756918858055830760946373372741144869620118570;
                W[2] = 0.233956967286345523694935171994775497405827802884605267655812659981957;
                W[3] = 0.233956967286345523694935171994775497405827802884605267655812659981957;
                W[4] = 0.180380786524069303784916756918858055830760946373372741144869620118570;
                W[5] = 0.085662246189585172520148071086366446763411250742021991199317719899473;
        }
        else if (order<=13)
        {
                //m = 7;
                G[0] = 0.025446043828620737736905157976074368799614531164691108225615448043468;
                G[1] = 0.129234407200302780068067613359605796462926176429304869940022324016285;
                G[2] = 0.2970774243113014165466967939615192683263089929503149368064783741026681;
                G[3] = 0.5;
                G[4] = 0.7029225756886985834533032060384807316736910070496850631935216258973319;
                G[5] = 0.870765592799697219931932386640394203537073823570695130059977675983715;
                G[6] = 0.974553956171379262263094842023925631200385468835308891774384551956532;
                W[0] = 0.064742483084434846635305716339541009164293701129973331988604319362328;
                W[1] = 0.139852695744638333950733885711889791243462532613299382268507016346809;
                W[2] = 0.190915025252559472475184887744487566939182541766931367375541725515353;
                W[3] = 0.208979591836734693877551020408163265306122448979591836734693877551020;
                W[4] = 0.190915025252559472475184887744487566939182541766931367375541725515353;
                W[5] = 0.139852695744638333950733885711889791243462532613299382268507016346809;
                W[6] = 0.064742483084434846635305716339541009164293701129973331988604319362328;
        }
        else if (order<=15)
        {
                //m = 8;
                G[0] = 0.019855071751231884158219565715263504785882382849273980864180111313788;
                G[1] = 0.101666761293186630204223031762084781581414134192017583964914852480391;
                G[2] = 0.2372337950418355070911304754053768254790178784398035711245714503637726;
                G[3] = 0.4082826787521750975302619288199080096666210935435131088414057631503978;
                G[4] = 0.5917173212478249024697380711800919903333789064564868911585942368496022;
                G[5] = 0.7627662049581644929088695245946231745209821215601964288754285496362274;
                G[6] = 0.898333238706813369795776968237915218418585865807982416035085147519609;
                G[7] = 0.980144928248768115841780434284736495214117617150726019135819888686212;
                W[0] = 0.050614268145188129576265677154981095057697045525842478529501849032370;
                W[1] = 0.111190517226687235272177997213120442215065435025624782362954644646808;
                W[2] = 0.156853322938943643668981100993300656630164499501367468845131972537478;
                W[3] = 0.181341891689180991482575224638597806097073019947165270262411533783343;
                W[4] = 0.181341891689180991482575224638597806097073019947165270262411533783343;
                W[5] = 0.156853322938943643668981100993300656630164499501367468845131972537478;
                W[6] = 0.111190517226687235272177997213120442215065435025624782362954644646808;
                W[7] = 0.050614268145188129576265677154981095057697045525842478529501849032370;
        }
        else if (order<=17)
        {
                //m = 9;
                G[0] = 0.015919880246186955082211898548163564975297599754037335224988344075460;
                G[1] = 0.081984446336682102850285105965132561727946640937662001947814010180272;
                G[2] = 0.1933142836497048013456489803292629076071396975297176535635935288593663;
                G[3] = 0.3378732882980955354807309926783316957140218696315134555864762615789067;
                G[4] = 0.5;
                G[5] = 0.6621267117019044645192690073216683042859781303684865444135237384210933;
                G[6] = 0.8066857163502951986543510196707370923928603024702823464364064711406337;
                G[7] = 0.918015553663317897149714894034867438272053359062337998052185989819728;
                G[8] = 0.984080119753813044917788101451836435024702400245962664775011655924540;
                W[0] = 0.040637194180787205985946079055261825337830860391205375355553838440343;
                W[1] = 0.090324080347428702029236015621456404757168910866020242249167953235679;
                W[2] = 0.130305348201467731159371434709316424885920102218649975969998501059805;
                W[3] = 0.156173538520001420034315203292221832799377430630952322777005582799572;
                W[4] = 0.165119677500629881582262534643487024439405391786344167296548248929201;
                W[5] = 0.156173538520001420034315203292221832799377430630952322777005582799572;
                W[6] = 0.130305348201467731159371434709316424885920102218649975969998501059805;
                W[7] = 0.090324080347428702029236015621456404757168910866020242249167953235679;
                W[8] = 0.040637194180787205985946079055261825337830860391205375355553838440343;
        }
        else 
        {
                //m = 10;
                G[0] = 0.013046735741414139961017993957773973285865026653808940384393966651702;
                G[1] = 0.067468316655507744633951655788253475736228492517334773739020134077313;
                G[2] = 0.160295215850487796882836317442563212115352644082595266167591405523721;
                G[3] = 0.2833023029353764046003670284171079188999640811718767517486492434281165;
                G[4] = 0.4255628305091843945575869994351400076912175702896541521460053732420482;
                G[5] = 0.5744371694908156054424130005648599923087824297103458478539946267579518;
                G[6] = 0.7166976970646235953996329715828920811000359188281232482513507565718835;
                G[7] = 0.839704784149512203117163682557436787884647355917404733832408594476279;
                G[8] = 0.932531683344492255366048344211746524263771507482665226260979865922687;
                G[9] = 0.986953264258585860038982006042226026714134973346191059615606033348298;
                W[0] = 0.033335672154344068796784404946665896428932417160079072564347440806706;
                W[1] = 0.074725674575290296572888169828848666201278319834713683917738634376619;
                W[2] = 0.109543181257991021997767467114081596229385935261338544940478271817600;
                W[3] = 0.134633359654998177545613460784734676429879969230441897900281638121077;
                W[4] = 0.147762112357376435086946497325669164710523358513426800677154014877998;
                W[5] = 0.147762112357376435086946497325669164710523358513426800677154014877998;
                W[6] = 0.134633359654998177545613460784734676429879969230441897900281638121077;
                W[7] = 0.109543181257991021997767467114081596229385935261338544940478271817600;
                W[8] = 0.074725674575290296572888169828848666201278319834713683917738634376619;
                W[9] = 0.033335672154344068796784404946665896428932417160079072564347440806706;
        }

        // compute number of gauss points : m^dim
  // see header file 
        // n = power(m,dim);            

        // set up all integration points 
        for (int i=0; i<n; ++i)
        {
                // compute integer coordinates of Gauss point from number
            FieldVector<int, dim> x (0);
                int z = i;
                for (int k=0; k<dim; ++k)
                {
                        x[k] = z%m;
                        z = z/m;
                }

                weight[i] = 1.0;
                for (int k=0; k<dim; k++) {
                        local[i][k] = G[x[k]];
                        weight[i]  *= W[x[k]];
                }
        }
}

template<class Domain, class RangeField, int dim, int order>
inline int GaussQuadrature<Domain,RangeField,dim,order>::nip ()
{
        return n;
}

template<class Domain, class RangeField, int dim, int order>
inline Domain GaussQuadrature<Domain,RangeField,dim,order>::ip (int i)
{
        return local[i];
}

template<class Domain, class RangeField, int dim, int order>
inline RangeField GaussQuadrature<Domain,RangeField,dim,order>::w (int i)
{
        return weight[i];
}

} // end namespace

#endif
