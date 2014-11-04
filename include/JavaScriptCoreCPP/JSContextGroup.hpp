/**
 * JavaScriptCoreCPP
 * Author: Matthew D. Langston
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _JAVASCRIPTCORECPP_JSCONTEXTGROUP_HPP_
#define _JAVASCRIPTCORECPP_JSCONTEXTGROUP_HPP_

#include "JavaScriptCoreCPP/JSClass.hpp"

#ifdef JAVASCRIPTCORECPP_PERFORMANCE_COUNTER_ENABLE
#include "JavaScriptCoreCPP/detail/JSPerformanceCounter.hpp"
#endif

#include <utility>

#ifdef JAVASCRIPTCORECPP_THREAD_SAFE
#include <mutex>
#endif

extern "C" struct JSContextGroupRef;

namespace JavaScriptCoreCPP {

class JSContext;
class JSClass;

/*!
  @class
  
  @discussion A JSContextGroup is an RAII wrapper around a
  JSContextGroupRef, the JavaScriptCore C API representation of a
  group that associates JavaScript contexts with one another.

  Scripts may execute concurrently with scripts executing in other
  contexts, and contexts within the same context group may share and
  exchange their JavaScript objects with one another.
  
  When JavaScript objects within the same context group are used in
  multiple threads, explicit synchronization is required.

  JSContextGroups are the only way to create a JSContext which
  represents a JavaScript execution context.

  JSContextGroups may be created with either the default or custom
  global objects. See the individual JSContextGroup constructors for
  more details.
*/
#ifdef JAVASCRIPTCORECPP_PERFORMANCE_COUNTER_ENABLE
class JSContextGroup final : public detail::JSPerformanceCounter<JSContextGroup> {
#else
class JSContextGroup final  {
#endif
  
 public:

  /*!
    @method
    
    @abstract Create a JavaScript context group. JSContexts within
    this context group may share and exchange JavaScript objects with
    one another.
  */
  JSContextGroup();
  
  /*!
    @method
    
    @abstract Create a JavaScript execution context within this
    context group. Scripts may execute in this context concurrently
    with scripts executing in other contexts.

    @discussion All JSContexts within this context group may share and
    exchange JavaScript values with one another.
    
    When JavaScript objects from the same context group are used in
    multiple threads, explicit synchronization is required.

    Providing an optional custom JSClass allows you to create a custom
    global object for this execution context. The default JSClass will
    create the global object populated with all of the standard
    built-in JavaScript objects, such as Object, Function, String, and
    Array

    @param global_object_class An optional JSClass used to create the
    global object.
  */
  JSContext CreateContext(const JSClass& global_object_class = {}) const;


  ~JSContextGroup();
  JSContextGroup(const JSContextGroup&);
  JSContextGroup(JSContextGroup&&);
  JSContextGroup& JSContextGroup::operator=(const JSContextGroup&) = delete;
  JSContextGroup& JSContextGroup::operator=(JSContextGroup&&) = delete;
  JSContextGroup& operator=(JSContextGroup);
  void swap(JSContextGroup&) noexcept;

 private:

  // For interoperability with the JavaScriptCore C API.
  explicit JSContextGroup(JSContextGroupRef js_context_group_ref);

  // JSContext needs access to operator JSContextGroupRef().
  friend class JSContext;
  
  operator JSContextGroupRef() const {
    return js_context_group_ref__;
  }
  
  // Prevent heap based objects.
  void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
  void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects

  friend void swap(JSContextGroup& first, JSContextGroup& second) noexcept;
  friend bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs);

  JSContextGroupRef js_context_group_ref__ { nullptr };

#undef JAVASCRIPTCORECPP_JSCONTEXTGROUP_LOCK_GUARD
#ifdef JAVASCRIPTCORECPP_THREAD_SAFE
                                                                    std::recursive_mutex       mutex__;
#define JAVASCRIPTCORECPP_JSCONTEXTGROUP_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#endif  // JAVASCRIPTCORECPP_THREAD_SAFE
};

// Return true if the two JSContextGroups are equal.
inline
bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs) {
  return lhs.js_context_group_ref__ == rhs.js_context_group_ref__;
}
  
// Return true if the two JSContextGroups are not equal.
inline
bool operator!=(const JSContextGroup& lhs, const JSContextGroup& rhs) {
  return ! (lhs == rhs);
}

} // namespace JavaScriptCoreCPP {

namespace std {
using JavaScriptCoreCPP::JSContextGroup;
template<>
void swap<JSContextGroup>(JSContextGroup& first, JSContextGroup& second) noexcept {
  first.swap(second);
}
}  // namespace std

#endif // _JAVASCRIPTCORECPP_JSCONTEXTGROUP_HPP_