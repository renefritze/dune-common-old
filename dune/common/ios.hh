#ifndef DUNE_IOS_HH
#define DUNE_IOS_HH
#include<ios>

namespace Dune{
   /** @addtogroup Common
   *
   * @{
   */
   /**
   * @file
   * @brief  Utility class for storing and restting old std::ios flags.
   * @author Markus Blatt
   */
  /**
   * @brief Utility class for storing and restting old std::ios flags.
   *
   * The constructor stores the flags currently set in the ios and the 
   * destructor sets these flags  in the ios object again.
   *
   */
  class IosFlagsRestorer
  {
  public:
    /**
     * @brief Constructor that stores the currently used flags.
     * @param ios_ The ios object whose flags are to be saved and restored.
     */
    IosFlagsRestorer(std::ios_base& ios_)
      : ios(ios_), oldflags(ios.flags())
    {}
  
    /**
     * @brief Destructor that restores the flags stored by the constructor.
     */
    ~IosFlagsRestorer()
    {
      ios.flags(oldflags);
    }
  private:
    /** @brief the ios object to restore the flags to.*/
    std::ios_base& ios;
    /** @brief The flags used when the constructor was called. */
    std::ios_base::fmtflags oldflags;
  };

  /** }@ */
}

#endif
