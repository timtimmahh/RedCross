//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_INCLUDE_PAIR_H
#define REDCROSS_INCLUDE_PAIR_H

#define P(first, second) Pair<typeof(first), typeof(second)>(first, second)

/**
 * Associate two objects with each other. Helper for Arduino UNO. C++ has pair
 * and tuple builtin, but the Arduino AVR framework doesn't include it.
 *
 * @tparam F type of the first object
 * @tparam S type of the second object
 */
template<class F, class S>
class Pair {
private:
  const F f_val;
  const S s_val;

public:
  /**
   * Constructor for Pair.
   *
   * @param first the first object
   * @param second the second object
   */
  Pair(F first, S second) : f_val(first), s_val(second) {}

  /**
   * Getter for the first object.
   *
   * @return the first object
   */
  F first() const { return f_val; }
  /**
   * Getter for the second object.
   *
   * @return the second object
   */
  S second() const { return s_val; }

  /**
   * Utility function for easily creating a Pair of objects.
   *
   * @tparam _T1 type of the first object
   * @tparam _T2 type of the second object
   * @param __x the first object
   * @param __y the second object
   * @return a Pair instance with the specified objects
   */
  template<class _T1, class _T2>
  inline Pair<_T1, _T2>
  static of(_T1 __x, _T2 __y)
  { return Pair<_T1, _T2>(__x, __y); }
};

#endif //REDCROSS_INCLUDE_PAIR_H
