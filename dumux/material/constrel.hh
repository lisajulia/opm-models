#ifndef DUNE_CONSTREL_HH
#define DUNE_CONSTREL_HH
/**
 * \ingroup material
 * \defgroup properties Constitutive relationships
 */

/** \ingroup properties
 * @brief base class for constitutive relationships
 */


class Solubility {
public:
//	virtual double operator() (double T=283.15) = 0; 
	
	double Henry (double T=283.15)
	{
		double celsius = T - 273.15;

		return (0.8942 + 1.47 * exp(-0.04394*celsius) )*1e-10; // [1/Pa]
	};
	
	double Antoine(double T=283.15)
	{
		const double constA = 8.19621; 
		const double constB = 1730.63;
		const double constC = 233.436;
		double celsius;
		double exponent, pwsat;

		celsius = T - 273.15;

		exponent = constA - (constB / (celsius + constC));

		pwsat = pow (10.0, exponent) *100; /*1mbar = 100Pa*/

		return(pwsat);
	};
	
	
	virtual double Xwg (double pg=1e5, double T=283.15)
	{
		double pwsat;
		double Xwg;
	
		pwsat = Antoine(T);
		Xwg = 0.0;//pwsat / pg;
		
		return(Xwg);
	};

	virtual double Xaw (double pg=1e5, double T=283.15)
	{
		double pag;
		double Xaw;
		double hagw;
		
		pag = pg * (1-Xwg(pg,T));
		hagw = Henry(T);
		Xaw = 0.0;//pag * hagw;
		
		return(Xaw);
	};

	
	Solubility()
	{}
//
	virtual ~Solubility()
	{  }
  
};

//class Henry : public Solubility
//{
//public:	
//	/** @brief Henry coefficient
//	 * @param T Temperature \f$ \left[ K \right] \f$
//	 * @return Henry coefficient \f$ \left[ 1/Pa \right] \f$
//	 */
//	virtual double operator() (double T) = 0; 
//	
//	virtual ~Henry() 
//	{ }
//};
//
///**\ingroup properties
// * @brief class for the Henry coefficient of a binary air/water system
// */
//class HenryWaterAir : public Henry
//{
//public:
//	double Henry(double T)
//	{
//		double pws;
//		return (pws) =  (0.8942 + 1.47 * exp(-0.04394*T) )*1e-10; // [1/Pa]
//	};
//
////	virtual double operator() (double T)
////	{
////		return (henry) =(0.8942 + 1.47 * exp(-0.04394*T) )*1e-10; // [1/Pa]
////	}
//};


#endif

