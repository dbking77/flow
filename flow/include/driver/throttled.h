/**
 * @copyright 2020 Fetch Robotics Inc.
 * @author Brian Cairl
 *
 * @file throttle.h
 */
#ifndef FLOW_DRIVER_THROTTLED_H
#define FLOW_DRIVER_THROTTLED_H

// Flow
#include <flow/captor.h>
#include <flow/dispatch.h>
#include <flow/driver/driver.h>

namespace flow
{
namespace driver
{

/**
 * @brief Throttled next element driving capture object
 *
 *        Captures the next oldest data element, limited to a max expected period. This means that some elements
 *        are skipped if the input rate indicated by data sequence stamps is higher than the throttled rate.
 * \n
 *        Establishes a sequencing range with <code>range.lower_stamp == range.upper_stamp</code> equal to
 *        the captured element stamp. Removes captured element from buffer.
 *
 * @tparam DispatchT  data dispatch type
 * @tparam LockPolicyT  a BasicLockable (https://en.cppreference.com/w/cpp/named_req/BasicLockable) object or NoLock or
 * PollingLock
 * @tparam ContainerT  underlying <code>DispatchT</code> container type
 * @tparam CaptureOutputT  captured output container type
 */
template <typename DispatchT, typename LockPolicyT = NoLock, typename ContainerT = DefaultContainer<DispatchT>>
class Throttled : public Driver<Throttled<DispatchT, LockPolicyT, ContainerT>>
{
public:
  /// Data stamp type
  using stamp_type = typename CaptorTraits<Throttled>::stamp_type;

  /// Data stamp duration type
  using offset_type = typename CaptorTraits<Throttled>::offset_type;

  /**
   * @brief Configuration constructor
   *
   * @param throttle_period  capture throttling period
   */
  explicit Throttled(const offset_type throttle_period);

  /**
   * @brief Configuration constructor
   *
   * @param throttle_period  capture throttling period
   * @param container  dispatch object container (non-default initialization)
   */
  explicit Throttled(const offset_type throttle_period, const ContainerT& container);

private:
  using PolicyType = Driver<Throttled<DispatchT, LockPolicyT, ContainerT>>;
  friend PolicyType;

  /**
   * @brief Checks if buffer is in ready state and captures data
   *
   * @param[out] output  output data iterator
   * @param[in,out] range  data capture/sequencing range
   *
   * @retval State::PRIMED  next element has been captured
   * @retval State::RETRY  Captor should continue waiting for messages after prime attempt
   */
  template <typename OutputDispatchIteratorT>
  inline State capture_driver_impl(OutputDispatchIteratorT output, CaptureRange<stamp_type>& range);

  /**
   * @copydoc Driver::dry_capture_policy_impl
   */
  inline State dry_capture_driver_impl(CaptureRange<stamp_type>& range) const;

  /**
   * @copydoc Driver::abort_policy_impl
   */
  inline void abort_driver_impl(const stamp_type& t_abort);

  /**
   * @copydoc Driver::reset_policy_impl
   */
  inline void reset_driver_impl();

  /// Capture throttling period
  offset_type throttle_period_;

  /// Previous captured element stamp
  stamp_type previous_stamp_;
};

}  // namespace driver


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
struct CaptorTraits<driver::Throttled<DispatchT, LockPolicyT, ContainerT>> : CaptorTraitsFromDispatch<DispatchT>
{
  /// Underlying dispatch container type
  using DispatchContainerType = ContainerT;

  /// Thread locking policy type
  using LockPolicyType = LockPolicyT;
};

}  // namespace flow

// Flow (implementation)
#include <flow/driver/impl/throttled.hpp>

#endif  // FLOW_DRIVER_THROTTLED_H
