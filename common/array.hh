#ifndef __DUNE_ARRAY_HH
#define __DUNE_ARRAY_HH

#include<iostream>
#include<iomanip>
#include<string>
#include<rpc/xdr.h>

namespace Dune 
{
  /** @addtogroup Common

  @{
  */

  //! A simple dynamic array class
  template <class T> 
  class Array {
  public:
  
      /** \brief An iterator to access all components of array.
       *
       * This iterator currently has no range checking! */
      class Iterator {  
      private:          
          T* p;         // Iterator ist Zeiger auf Feldelement

      public:        
          //! Default constructor
          Iterator();
 
          //! equality of two iterators
          bool operator!= (Iterator x);

          //! inequality of two iterators
          bool operator== (Iterator x);

          //! prefix increment
          Iterator operator++ ();   // prefix Stroustrup p. 292
          
          //! postfix increment
          Iterator operator++ (int);// postfix

          //! dereferencing 
          T& operator* () const;

          //! selector
          T* operator-> () const; // Stroustrup p. 289

          friend class Array<T>;
        } ;

        //! return iterator refering to first element in array
        Iterator begin () const; 

        //! return iterator refering to on past the last element of the array
        Iterator end () const;

        //! make empty array
        Array();
        
        //! make array with m components
        explicit Array(int m);

        //! copy constructor making shallow copy
        Array (const Array<T>&);

        //! destructor
        ~Array();

        //! assignment of two arrays
        Array<T>& operator= (const Array<T>&);

        //! assign value to all components
        Array<T>& operator= (const T& a);

        //! reallocate array with size m
        void resize (int m);

      /** \todo This the same as resize! */
      void realloc (int m) {resize(m);}

      //! just for some tests 
      void swap ( Array<T> &copy)
      {
        T *tmp = copy.p;
        copy.p = p;
        p = tmp;
       
        int fake=copy.n;
        copy.n = n;
        n = fake;
      }
      
      //! return number of components in array
      int size () const;

      //! Sets all entries to the same value
      void set(const T& v) {
          for (int i=0; i<size(); i++)
              (*this)[i] = v;
      }

      //! random access operator
      T& operator[](int i);

      //! Const random access operator
      const T& operator[](int i) const;

      //! export base type of array
      typedef T MemberType;  

      /** \brief Print contents of the array to cout.
       *
       * @param k Number of elements per printed row.
       * @param s Name of the array appearing in the first line.
       * @param row String being printed in each line.
       */
      void print (int k, std::string s, std::string row)
        {
          char buf[96];
          std::cout << s << " size=" << n << " {" << std::endl;
          for (int i=0; i<n/k; i++)
                {
                  sprintf(buf,"%4d",k*i);
                  std::cout << buf << " " << row << " ";
                  for (int j=i*k; j<(i+1)*k; j++)
                        {
                          sprintf(buf,"%10.3E ",p[j]);
                          std::cout << buf;
                        }
                  std::cout << std::endl;
                }
          int i = (n/k)*k;
          if (i<n)
                {
                  sprintf(buf,"%4d",i);
                  std::cout << buf << " " << row << " ";
                  for (int j=i; j<n; j++)
                        {
                          sprintf(buf,"%10.3E ",p[j]);
                          std::cout << buf;
                        }
                  std::cout << std::endl;
                }
          std::cout << "}" << std::endl;          
        }

      /** \todo Please doc me! 
       */
      bool processXdr(XDR *xdrs)
      {
        if(xdrs != NULL)
        {
          int len = n;
          xdr_int( xdrs, &len );
          if(len != n) resize(len);
          xdr_vector(xdrs,(char *) p,n,sizeof(T),(xdrproc_t)xdr_double);
          return true;
        }
        else 
          return false;
      }

  protected:
      int n;  // Anzahl Elemente; n=0 heisst, dass kein array allokiert ist!
      T *p;   // Zeiger auf built-in array
  } ;


  // Iterator interface
  template<class T>
  inline typename Array<T>::Iterator Array<T>::begin () const
  {
        Iterator tmp;  // hat Zeiger 0
        tmp.p = p;     // Zeiger auf Feldelement 0
        return tmp;
  }
  
  template<class T>
  inline typename Array<T>::Iterator Array<T>::end () const
  {
        Iterator tmp;
        tmp.p = &(p[n]); // Zeiger auf Feldelement NACH letztem !
        return tmp;      // das funktioniert: Stroustrup p. 92.
  }

  // Destruktor
  template <class T>
  inline Array<T>::~Array () 
  { 
        if (n>0) delete[] p; 
  }
  
  // Konstruktoren
  template <class T>
  inline Array<T>::Array ()
  {
        n = 0;
  }
  
  template <class T>
  inline Array<T>::Array (int m)
  {
        n = m;
        if (n<=0)
          {
                n = 0;
                return;
          }
        try {
          p = new T[n];
        }
        catch (std::bad_alloc) {
          std::cerr << "nicht genug Speicher!" << std::endl;
          throw;
        }
  }

  template <class T>
  inline void Array<T>::resize (int m)
  {
        if (m!=n)
          {
                if (n>0) 
                {
                  delete[] p;
                  p = NULL;
                }
                n = m;
                if (n==0) 
                {
                  p = NULL;
                  return;
                }
                try {
                  p = new T[n];
                }
                catch (std::bad_alloc) {
                  std::cerr << "nicht genug Speicher!" << std::endl;
                  throw;
                }
          }
  }

  // Copy-Konstruktor
  template <class T>
  inline Array<T>::Array (const Array<T>& a)
  {
        // Erzeuge Feld mit selber Groesse wie a
        n = a.n;
        if (n>0)
          {
                try {
                  p = new T[n];
                }
                catch (std::bad_alloc) {
                  std::cerr << "nicht genug Speicher!" << std::endl;
                  throw;
                }
          }

        // und kopiere Elemente
        for (int i=0; i<n; i=i+1) p[i]=a.p[i];
  }

  // Zuweisung
  template <class T>
  inline Array<T>& Array<T>::operator= (const Array<T>& a)
  {
        if (&a!=this) // nur bei verschiedenen Objekten was tun
          {
                if (n!=a.n)
                  {
                        // allokiere fuer this ein Feld der Groesse a.n
                        if (n>0) delete[] p; // altes Feld loeschen
                        n = a.n;
                        if (n>0)
                          {
                                try {
                                  p = new T[n];
                                }
                                catch (std::bad_alloc) {
                                  std::cerr << "nicht genug Speicher!" << std::endl;
                                  throw;
                                }
                          }
                  }
                for (int i=0; i<n; i++) p[i]=a.p[i];
          }
        return *this; // Gebe Referenz zurueck damit a=b=c; klappt
  }

  // Zuweisung mit member type
  template <class T>
  inline Array<T>& Array<T>::operator= (const T& a)
  {
        for (int i=0; i<n; i++) p[i]=a;
        return *this; // Gebe Referenz zurueck damit a=b=c; klappt
  }
  
  // Indizierung
  template <class T>
  inline T& Array<T>::operator[] (int i)
  {
        return p[i];
  }
  template <class T>
  inline const T& Array<T>::operator[] (int i) const
  {
        return p[i];
  }

  // Groesse
  template <class T>
  inline int Array<T>::size () const
  { 
        return n; 
  }

  // Ausgabe
  template <class T>
  std::ostream& operator<< (std::ostream& s, Array<T>& a)
  {
        s << "array " << a.size() << " elements = [" << std::endl;
        for (int i=0; i<a.size(); i++)
          s << "    " << i << "  " << a[i] << std::endl;
        s << "]" << std::endl;
        return s;
  }

  template<class T>
  inline Array<T>::Iterator::Iterator () 
  {
        p=0; // nicht initialisierter Iterator
  }

  template<class T>
  inline bool Array<T>::Iterator::operator!= 
  (Array<T>::Iterator x)
  {
        return p != x.p;
  }

  template<class T>
  inline bool Array<T>::Iterator::operator== 
  (Array::Iterator x)
  {
        return p == x.p;
  }
                
  template<class T>
  inline typename Array<T>::Iterator Array<T>::Iterator::operator++ () // prefix
  {
        p++;  // C Zeigerarithmetik: p zeigt auf naechstes Feldelement
        return *this;
  }

  template<class T>
  inline typename Array<T>::Iterator Array<T>::Iterator::operator++ (int) // postfix
  {
        Iterator tmp = *this;
        ++*this;
        return tmp;
  }

  template<class T>
  inline T& Array<T>::Iterator::operator* () const
  {
        return *p;
  }
                
  template<class T>
  inline T* Array<T>::Iterator::operator-> () const
  {
        return p;
  }

  /** @} */

} // end namespace Dune

#endif
