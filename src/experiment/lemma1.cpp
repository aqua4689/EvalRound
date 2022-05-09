
#include "experiment/simple_parameter.h"
#include "experiment/util.h"

#include "impl/message.h"
#include "impl/simple_plaintext.h"

#include <iostream>

int main()
{
  // ||e * pt|| ~= sqrt(N/12) * ||pt||
  Message <LOGN> z, V;
  SimplePlaintext<LOGN> pt, pt_v_raw, pt_v, e, res;

  set_test_message<LOGN>(z);
  encode<LOGN>(z, Delta, pt);
  double expected = sqrt(N /12.0) * norm(pt);
  std::cout << expected << std::endl;
  std::cout << std::endl;

  for(int r = 0; r < 100; ++r) {
    set_random_message<LOGN>(V);
    encode_raw<LOGN>(V, Delta, pt_v_raw);
    encode<LOGN>(V, Delta, pt_v);
    sub<LOGN>(pt_v_raw, pt_v, e);
    conv<LOGN>(e, pt, res);

    double measured = norm<LOGN>(res);
    std::cout << measured << std::endl;
    //std::cout << "Measured : " << measured << std::endl;
    //std::cout << "Expected : " << expected << std::endl;
    //std::cout << "Measured / Expected : " << (measured / expected) << std::endl;
  }
}
