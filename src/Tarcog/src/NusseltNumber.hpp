#ifndef NUSSELTNUMBER_H
#define NUSSELTNUMBER_H

namespace Tarcog {
  class CNusseltNumberStrategy {
  public:
    virtual double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  protected:
    double pos( double const t_Value );
  };

  class CNusseltNumber0To60 : public CNusseltNumberStrategy {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };

  class CNusseltNumber60 : public CNusseltNumberStrategy {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };

  class CNusseltNumber60To90 : public CNusseltNumberStrategy {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };

  class CNusseltNumber90to180 : public CNusseltNumberStrategy {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };

  class CNusseltNumber90 : public CNusseltNumberStrategy {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };

  class CNusseltNumber {
  public:
    double calculate( double const t_Tilt, double const t_Ra, double const t_Asp );
  };
}


#endif