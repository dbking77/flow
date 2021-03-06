/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Levon Avagyan, Brian Cairl
 *
 * @file before.h
 */
#ifndef FLOW_FOLLOWER_ANY_BEFORE_H
#define FLOW_FOLLOWER_ANY_BEFORE_H

// Flow
#include <flow/follower/follower.h>

namespace flow
{
namespace follower
{

/**
 * @brief Captures all data elements from a delay before the driving sequencing stamp
 *
 *        This capture buffer will capture data which is behind the driving upper
 *        sequence stamp (<code>range.upper_stamp</code>) by some sequencing delay
 *        w.r.t a driver-provided target time. It will return all data at and before
 *        that sequencing boundary that has not previously been captured.
 * \n
 *        This capture buffer is always ready, and will always return with a PRIMED state,
 *        regardless of whether or not there is data available to capture.
 * \n
 *        <b>Data removal:</b> Captor will remove all data before the driving time
 *        message minus the delay
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 *
 * @warn This captor WILL NOT behave deterministically if all data is not available before capture time minus
 *       the specified delay. As such, setting the delay properly will alleviate non-deterministic behavior.
 *       This is the only <i>optional</i> captor, and should be used with great caution.
 */
template <typename DispatchT, typename LockPolicyT = NoLock, typename ContainerT = DefaultContainer<DispatchT>>
class AnyBefore : public Follower<AnyBefore<DispatchT, LockPolicyT, ContainerT>>
{
public:
  /// Data stamp type
  using stamp_type = typename CaptorTraits<AnyBefore>::stamp_type;

  /// Data stamp duration type
  using offset_type = typename CaptorTraits<AnyBefore>::offset_type;

  /**
   * @brief Setup constructor
   * @param delay  the delay with which to capture
   */
  explicit AnyBefore(const offset_type& delay);

  /**
   * @brief Setup constructor
   * @param delay  the delay with which to capture
   * @param container  dispatch object container (non-default initialization)
   */
  AnyBefore(const offset_type& delay, const ContainerT& container);

private:
  using PolicyType = Follower<AnyBefore<DispatchT, LockPolicyT, ContainerT>>;
  friend PolicyType;

  /**
   * @brief Checks if buffer is in ready state and collects data based on a target time
   *
   * @param[out] output  output data iterator
   * @param[in] range  data capture/sequencing range
   *
   * @retval PRIMED  always
   */
  template <typename OutputDispatchIteratorT>
  inline State capture_follower_impl(OutputDispatchIteratorT output, const CaptureRange<stamp_type>& range);

  /**
   * @copydoc Follower::dry_capture_policy_impl
   */
  inline State dry_capture_follower_impl(const CaptureRange<stamp_type>& range) const;

  /**
   * @copydoc Follower::abort_policy_impl
   */
  inline void abort_follower_impl(const stamp_type& t_abort);

  /**
   * @copydoc Follower::reset_policy_impl
   */
  inline void reset_follower_impl() noexcept(true) {}

  /// Capture delay
  offset_type delay_;
};

}  // namespace follower


/**
 * @copydoc CaptorTraits
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 * @tparam CaptureOutputT  output capture container type
 */
template <typename DispatchT, typename LockPolicyT, typename ContainerT>
struct CaptorTraits<follower::AnyBefore<DispatchT, LockPolicyT, ContainerT>> : CaptorTraitsFromDispatch<DispatchT>
{
  /// Underlying dispatch container type
  using DispatchContainerType = ContainerT;

  /// Thread locking policy type
  using LockPolicyType = LockPolicyT;
  ;
};

}  // namespace flow

// Flow (implementation)
#include <flow/follower/impl/any_before.hpp>

#endif  // FLOW_FOLLOWER_ANY_BEFORE_H
