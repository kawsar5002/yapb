//
// Yet Another POD-Bot, based on PODBot by Markus Klinge ("CountFloyd").
// Copyright (c) YaPB Development Team.
//
// This software is licensed under the BSD-style license.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://yapb.ru/license
//

#pragma once

#include <new>

#include <crlib/cr-basic.h>
#include <crlib/cr-movable.h>
#include <crlib/cr-platform.h>

CR_NAMESPACE_BEGIN

// default allocator for cr-objects
class Allocator : public Singleton <Allocator> {
public:
   Allocator () = default;
   ~Allocator () = default;

public:
   template <typename T> T *allocate (const size_t length = 1) {
      auto ptr = reinterpret_cast <T *> (calloc (length, sizeof (T)));

      if (!ptr) {
         plat.abort ();
      }

      // calloc on linux with debug enabled doesn't always zero out memory
#if defined (CR_DEBUG) && !defined (CR_WINDOWS)
      auto *zeroing = reinterpret_cast <uint8 *> (ptr);

      for (size_t i = 0; i < length; ++i) {
         zeroing[i] = 0;
      }
#endif
      return ptr;
   }

   template <typename T> void deallocate (T *ptr) {
      if (ptr) {
         free (reinterpret_cast <T *> (ptr));
      }
   }

public:
   template <typename T, typename ...Args> void construct (T *d, Args &&...args) {
      new (d) T (cr::forward <Args> (args)...);
   }

   template <typename T> void destruct (T *d) {
      d->~T ();
   }

public:
   template <typename T, typename ...Args> T *create (Args &&...args) {
      auto d = allocate <T> ();
      new (d) T (cr::forward <Args> (args)...);

      return d;
   }

   template <typename T> void destroy (T *ptr) {
      if (ptr) {
         destruct (ptr);
         deallocate (ptr);
      }
   }
};

static auto &alloc = Allocator::get ();

CR_NAMESPACE_END