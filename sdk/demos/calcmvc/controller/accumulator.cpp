/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 */

#include <tnt/component.h>
#include <tnt/componentfactory.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <model/accumulator.h>

namespace controller
{
  class Accumulator : public tnt::Component
  {
    public:
      unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
  };

  static tnt::ComponentFactoryImpl<Accumulator> factory("controller/accumulator");

  unsigned Accumulator::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
  {
    TNT_SESSION_GLOBAL_VAR(model::Accumulator, accumulator, ());

    if (qparam.arg<bool>("increment"))
      accumulator.increment();

    if (qparam.arg<bool>("decrement"))
      accumulator.decrement();

    return DECLINED;
  }
}
