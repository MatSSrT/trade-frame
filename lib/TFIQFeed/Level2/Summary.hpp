/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Summary.hpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 17, 2021 11:45
 */

#pragma once

#include <map>
#include <string>

#include <TFTimeSeries/DatedDatum.h>

#include <TFIQFeed/Level2/Dispatcher.h>

// TODO: implement one per symbol
// for consistency, needs to go through proivider, and is processed through Symbol
// so ... keep this for the Visual Ladders for fast access,
//    will need a different version of this for Provider access
// NOTE: rather than these suggestions, is superceded in separate Symbols.hpp

// TODO - make a choice
//    pass in dispatch lambdas?
//        then allows multi-purpose lambdas for chart plus trading, etc
//    CRTP with the graph code?  CRTP with multi-purpose code:  chart plus trading?
//       then allows intermediate multi-purpose dispatch via very fast calls

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

class Summary
: public Dispatcher<Summary>
{
  friend Dispatcher<Summary>;
public:

  using fVolumeAtPrice_t = std::function<void(double,int)>;

  Summary( const std::string& sWatch );
  virtual ~Summary();

  void Set( fVolumeAtPrice_t&& fBid, fVolumeAtPrice_t&& fAsk );

  void EmitMarketMakerMaps();

  void Connect();
  void Disconnect();

protected:

  // called by Network via CRTP
  //void OnNetworkConnected();
  //void OnNetworkDisconnected();
  //void OnNetworkError( size_t e );

  void OnL2Initialized();

  void OnMBOAdd( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& );
  void OnMBOSummary( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& );
  void OnMBOUpdate( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& );
  void OnMBODelete( const ou::tf::iqfeed::l2::msg::OrderDelete::decoded& );

private:

  fVolumeAtPrice_t m_fBidVolumeAtPrice;
  fVolumeAtPrice_t m_fAskVolumeAtPrice;

  using volume_t = ou::tf::Trade::volume_t;

  std::string m_sWatch;

  struct Order {
    char chOrderSide;
    double dblPrice;
    volume_t nQuantity;
    uint64_t nPriority;
    uint8_t nPrecision;
    // ptime, if needed
    // note: there is no MarketMaker in messages with an order ID
    Order( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg )
    : chOrderSide( msg.chOrderSide ),
      dblPrice( msg.dblPrice ), nQuantity( msg.nQuantity ),
      nPriority( msg.nPriority ), nPrecision( msg.nPrecision )
    { assert( 0 == msg.sMarketMaker.size() ); }
  };

  using mapOrder_t = std::map<uint64_t,Order>; // key is order id
  mapOrder_t m_mapOrder;

  struct Auction {
    // maintain set of orders?
    volume_t nQuantity;
    int nOrders;
    Auction( volume_t nQuantity_ )
    : nQuantity( nQuantity_ ), nOrders( 1 )  {}
    Auction( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& msg )
    : nQuantity( msg.nQuantity ), nOrders( 1 ) {}
  };

  using mapAuction_t = std::map<double,Auction>;  // key is price
  mapAuction_t m_mapAuctionAsk;
  mapAuction_t m_mapAuctionBid;

  // updated with OnMBOOrderArrival
  struct price_level {
    double price;
    volume_t volume;
    price_level(): price {}, volume {} {}
    price_level( double price_, volume_t volume_ )
    : price( price_ ), volume( volume_ ) {}
  };
  using mapMM_t = std::map<std::string,price_level>; // key=mm, value=price,volume
  mapMM_t m_mapMMAsk;
  mapMM_t m_mapMMBid;

  // OnMBOSummary, OnMBOUpdate (for Nasdaq LII )
  void OnMBOOrderArrival( const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& );
  void MMAuction_Update(
    const ou::tf::iqfeed::l2::msg::OrderArrival::decoded&,
    fVolumeAtPrice_t&,
    mapMM_t&, mapAuction_t& );
  void MMAuction_Delete(
    const ou::tf::iqfeed::l2::msg::OrderDelete::decoded&,
    fVolumeAtPrice_t&,
    mapMM_t&, mapAuction_t& );

  void AuctionAdd(
    const ou::tf::iqfeed::l2::msg::OrderArrival::decoded&,
    fVolumeAtPrice_t&,
    mapAuction_t& );
  void AuctionUpdate(
    mapAuction_t& map,
    Order& order,
    const ou::tf::iqfeed::l2::msg::OrderArrival::decoded& ,
    fVolumeAtPrice_t&
    );
  void AuctionDel( mapAuction_t& map, const Order&, fVolumeAtPrice_t& );

};

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou