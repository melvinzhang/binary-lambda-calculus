//== llvm/ADT/IntrusiveRefCntPtr.h - Smart Refcounting Pointer ---*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines IntrusiveRefCntPtr, a template class that
// implements a "smart" pointer for objects that maintain their own
// internal reference count, and RefCountedBase/RefCountedBaseVPTR, two
// generic base classes for objects that wish to have their lifetimes
// managed using reference counting.
//
// IntrusiveRefCntPtr is similar to Boost's intrusive_ptr with added
// LLVM-style casting.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
/// IntrusiveRefCntPtr - A template class that implements a "smart pointer"
///  that assumes the wrapped object has a reference count associated
///  with it that can be managed via calls to
///  IntrusivePtrAddRef/IntrusivePtrRelease.  The smart pointers
///  manage reference counts via the RAII idiom: upon creation of
///  smart pointer the reference count of the wrapped object is
///  incremented and upon destruction of the smart pointer the
///  reference count is decremented.  This class also safely handles
///  wrapping NULL pointers.
///
/// Reference counting is implemented via calls to
///  Obj->Retain()/Obj->Release(). Release() is required to destroy
///  the object when the reference count reaches zero. Inheriting from
///  RefCountedBase/RefCountedBaseVPTR takes care of this
///  automatically.
//===----------------------------------------------------------------------===//
  template <typename T>
  class IntrusiveRefCntPtr {
    T* Obj = nullptr;

  public:
    typedef T element_type;

    explicit IntrusiveRefCntPtr() = default;

    IntrusiveRefCntPtr(T* obj) : Obj(obj) {
      retain();
    }

    IntrusiveRefCntPtr(const IntrusiveRefCntPtr& S) : Obj(S.Obj) {
      retain();
    }

    IntrusiveRefCntPtr(IntrusiveRefCntPtr&& S) : Obj(S.Obj) {
      S.Obj = nullptr;
    }

    template <class X>
    IntrusiveRefCntPtr(IntrusiveRefCntPtr<X>&& S) : Obj(S.get()) {
      S.Obj = nullptr;
    }

    template <class X>
    IntrusiveRefCntPtr(const IntrusiveRefCntPtr<X>& S)
      : Obj(S.get()) {
      retain();
    }

    IntrusiveRefCntPtr& operator=(IntrusiveRefCntPtr S) {
      swap(S);
      return *this;
    }

    ~IntrusiveRefCntPtr() { release(); }

    T& operator*() const { return *Obj; }

    T* operator->() const { return Obj; }

    T* get() const { return Obj; }

    explicit operator bool() const { return Obj; }

    void swap(IntrusiveRefCntPtr& other) {
      T* tmp = other.Obj;
      other.Obj = Obj;
      Obj = tmp;
    }

    void reset() {
      release();
      Obj = nullptr;
    }

    void resetWithoutRelease() {
      Obj = nullptr;
    }

  private:
    void retain() { if (Obj) Obj->retain(); }
    void release() { if (Obj) Obj->release(); }

    template <typename X>
    friend class IntrusiveRefCntPtr;
  };
