/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    OptionRegistry.hpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrader
 * Created: March 4, 2023 19:42:36
 */

#pragma once

#include <functional>

#include <TFTrading/Position.h>

#include <TFOptions/Option.h>
#include <TFOptionCombos/Combo.h>

#include "OptionStatistics.hpp"

namespace ou {
namespace tf {
  class TreeItem;
}
}

class OptionRegistry {
public:

  using pWatch_t     = ou::tf::option::Option::pWatch_t;
  using pOption_t    = ou::tf::option::Option::pOption_t;
  using pPosition_t  = ou::tf::Position::pPosition_t;

  using fRegisterOption_t = std::function<void(pOption_t&)>;
  using fStartCalc_t = std::function<void(pOption_t,pWatch_t)>; // option, underlying
  using fStopCalc_t =  std::function<void(pOption_t,pWatch_t)>; // option, underlying

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;
  using fSetChartDataView_t = std::function<void(pChartDataView_t)>;

  OptionRegistry(
      fRegisterOption_t&& fRegisterOption
    , fStartCalc_t&& fStartCalc
    , fStopCalc_t&& fStopCalc
    , fSetChartDataView_t&& fSetChartDataView
    );

  ~OptionRegistry();

  void AssignWatchUnderlying( pWatch_t pWatchUnderlying ) {
    m_pWatchUnderlying = pWatchUnderlying;
  }

  void SetTreeItem( ou::tf::TreeItem* ptiParent ) {
    m_ptiParent = ptiParent;
  }

  void Add( pOption_t pOption, pPosition_t pPosition, const std::string& sLegName, ou::tf::option::Combo::vMenuActivation_t&& ma );
  void Remove( pOption_t pOption );

protected:
private:

  pWatch_t m_pWatchUnderlying;

  fRegisterOption_t m_fRegisterOption;
  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;
  fSetChartDataView_t m_fSetChartDataView;

  ou::tf::TreeItem* m_ptiParent;

  using pOptionStatistics_t = OptionStatistics::pOptionStatistics_t;
  using mapOptionStatistics_t = std::map<std::string,pOptionStatistics_t>; // for m_fStartCalc, m_fStopCalc
  mapOptionStatistics_t m_mapOptionStatistics;

};