#ifndef BJSON_EXPORT_H
#define BJSON_EXPORT_H

#if defined(__GNUC__) || defined(__clang__)
  #define BJSON_EXPORT __attribute__((visibility("default")))
#else
  #define BJSON_EXPORT
#endif

#endif
