#include <iostream>
#include <main.h>
#include <test.h>

namespace test 
{
	namespace m
	{
		template<typename T>
		class Class
		{
          private:
	  	void testFunction(int);

              template<typename Q, int N>
              class JJJ
              {
	      	void testFunction(int);

                template<class C>
                Q fff(Q q, C c, ...);
              };

          public:
	  	
				template<typename D>
				volatile T function(D d) const;
		};
	}
}

namespace other
{
	void test(int a);
}

template<typename T>
void test::m::Class<T>::testFunction(int)
{}

int funkcja()
{
	return 0;
}

int funk();

