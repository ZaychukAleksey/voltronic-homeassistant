#include "pi18_protocol_adapter.hh"

#include <format>

#include "spdlog/spdlog.h"
#include "exceptions.h"

namespace {

std::string Concatenate(const std::vector<std::string>& strings, char separator = ',') {
  std::string result;
  for (auto& s : strings) {
    if (!result.empty()) {
      result += separator;
    }
    result.append(s);
  }
  return result;
}

BatteryType GetBatteryType(int type) {
  switch (type) {
    case 0: return BatteryType::kAgm;
    case 1: return BatteryType::kFlooded;
    case 2: return BatteryType::kUser;
    default: throw std::runtime_error(std::format("Unknown battery type: {}", type));
  }
}

/// Opposite to the previous function.
int GetBatteryType(BatteryType t) {
  switch (t) {
    case BatteryType::kAgm: return 0;
    case BatteryType::kFlooded: return 1;
    case BatteryType::kUser: return 2;
    default: throw std::runtime_error(std::format("Unexpected BatteryType: {}", ToString(t)));
  }
}

InputVoltageRange GetInputVoltageRange(int type) {
  switch (type) {
    case 0: return InputVoltageRange::kAppliance;
    case 1: return InputVoltageRange::kUps;
    default: throw std::runtime_error(std::format("Unknown InputVoltageRange: {}", type));
  }
}

/// Opposite to the previous function.
int GetInputVoltageRange(InputVoltageRange r) {
  switch (r) {
    case InputVoltageRange::kAppliance: return 0;
    case InputVoltageRange::kUps: return 1;
  }
  throw std::runtime_error(std::format("Unexpected InputVoltageRange: {}", ToString(r)));
}

OutputSourcePriority GetOutputSourcePriority(int type) {
  switch (type) {
    case 0: return OutputSourcePriority::kSolarUtilityBattery;
    case 1: return OutputSourcePriority::kSolarBatteryUtility;
    default: throw std::runtime_error(std::format("Unknown OutputSourcePriority: {}", type));
  }
}

/// Opposite to the previous function.
std::string_view GetOutputSourcePriority(OutputSourcePriority p) {
  switch (p) {
    case OutputSourcePriority::kSolarUtilityBattery: return "0";
    case OutputSourcePriority::kSolarBatteryUtility: return "1";
    default:
      throw std::runtime_error(std::format("Unexpected OutputSourcePriority: {}", ToString(p)));
  }
}

ChargerPriority GetChargerPriority(int type) {
  switch (type) {
    case 0: return ChargerPriority::kSolarFirst;
    case 1: return ChargerPriority::kSolarAndUtility;
    case 2: return ChargerPriority::kOnlySolar;
    default: throw std::runtime_error(std::format("Unknown ChargerPriority: {}", type));
  }
}

/// Opposite to the previous function.
std::string_view GetChargerPriority(ChargerPriority p) {
  switch (p) {
    case ChargerPriority::kSolarFirst: return "0";
    case ChargerPriority::kSolarAndUtility: return "1";
    case ChargerPriority::kOnlySolar: return "2";
    default: throw std::runtime_error(std::format("Unexpected ChargerPriority: {}", ToString(p)));
  }
}

MachineType GetMachineType(int type) {
  switch (type) {
    case 0: return MachineType::kOffGrid;
    case 1: return MachineType::kGridTie;
    default: throw std::runtime_error(std::format("Unknown MachineType: {}", type));
  }
}

Topology GetTopology(int type) {
  switch (type) {
    case 0: return Topology::kTransformless;
    case 1: return Topology::kTransformer;
    default: throw std::runtime_error(std::format("Unknown Topology: {}", type));
  }
}

OutputMode GetOutputMode(int type) {
  switch (type) {
    case 0: return OutputMode::kSingle;
    case 1: return OutputMode::kParallel;
    case 2: return OutputMode::kPhase1Of3;
    case 3: return OutputMode::kPhase2Of3;
    case 4: return OutputMode::kPhase3Of3;
    default: throw std::runtime_error(std::format("Unknown OutputMode: {}", type));
  }
}

constexpr DeviceMode GetDeviceMode(std::string_view mode) {
  if (mode == "00") return DeviceMode::kPowerOn;
  if (mode == "01") return DeviceMode::kStandby;
  if (mode == "02") return DeviceMode::kBypass;
  if (mode == "03") return DeviceMode::kBattery;
  if (mode == "04") return DeviceMode::kFault;
  if (mode == "05") return DeviceMode::kHybrid;
  throw std::runtime_error(std::format("Unknown device mode: {}", mode));
}

SolarPowerPriority GetSolarPowerPriority(int type) {
  switch (type) {
    case 0: return SolarPowerPriority::kBatteryLoadUtility;
    case 1: return SolarPowerPriority::kLoadBatteryUtility;
    default: throw std::runtime_error(std::format("Unknown SolarPowerPriority: {}", type));
  }
}

/// Opposite to the previous function.
int GetSolarPowerPriority(SolarPowerPriority p) {
  switch (p) {
    case SolarPowerPriority::kBatteryLoadUtility: return 0;
    case SolarPowerPriority::kLoadBatteryUtility: return 1;
    default: throw std::runtime_error(std::format("Unexpected SolarPowerPriority: {}", ToString(p)));
  }
}

}  // namespace


Pi18ProtocolAdapter::Pi18ProtocolAdapter(const SerialPort& port)
    : ProtocolAdapter(port) {}

std::string Pi18ProtocolAdapter::GetSerialNumber() {
  // Response: ^D025LLXXXXXXXXXXXXXXXXXXXX<CRC><cr>
  auto response = GetSeriesNumberRaw();
  if (response.length() != 22) {
    throw std::runtime_error(std::format("Unexpected serial number response length: {}", response));
  }
  const auto serial_number_length = 10 * utils::AsDigit(response[0]) + utils::AsDigit(response[1]);
  if (serial_number_length < 1 || serial_number_length > 20) {
    throw std::runtime_error(std::format("Unexpected serial number length: {}. Response: {}",
                                         serial_number_length, response));
  }
  return response.substr(2, serial_number_length);
}

std::string Pi18ProtocolAdapter::GetGeneratedEnergyOfYearRaw(std::string_view year) {
  return Query(std::format("^P009EY{}", year), "^D011");
}

std::string Pi18ProtocolAdapter::GetGeneratedEnergyOfMonthRaw(std::string_view year,
                                                              std::string_view month) {
  return Query(std::format("^P011EM{}{}", year, month), "^D011");
}

std::string Pi18ProtocolAdapter::GetGeneratedEnergyOfDayRaw(
    std::string_view year, std::string_view month, std::string_view day) {
  return Query(std::format("^P013ED{}{}{}", year, month, day), "^D011");
}

/// Depending of inverter's nominal battery voltage
void Pi18ProtocolAdapter::SetBatteryStopChargingVoltageWithGrid(auto battery_nominal_voltage,
                                                                int value) {
//  if (!battery_stop_charging_voltage_with_grid_) {
//    // TODO fix
//    std::function<bool(int)> callback = [this](double) { return true; };
//    battery_stop_charging_voltage_with_grid_ = mqtt::BatteryStopChargingVoltageWithGrid::Create(
//        battery_nominal_voltage, std::move(callback));
//  }
//
//  battery_stop_charging_voltage_with_grid_->Update(value);
}

void Pi18ProtocolAdapter::GetRatedInfo() {
  // Special case. According to the protocol, the length is 85. But my inverter returns 89.
  // Therefore I can't check it as a prefix and have to skip it here.
  auto response = GetRatedInformationRaw().substr(2);

  // Response according to the protocol:
  // AAAA,BBB,CCCC,DDD,EEE,FFFF,GGGG,HHH,III,JJJ,KKK,LLL,MMM,N,OO,PPP,Q,R,S,T,U,V,W,Z,a
  // But my inverter for some reason returns an extra argument at the end of the format.
  int data[26];
  const auto n_args = sscanf(response.c_str(),
         "%4d,%3d,%4d,%3d,%3d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%1d,%d,%3d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d",
         &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8],
         &data[9], &data[10], &data[11], &data[12], &data[13], &data[14], &data[15], &data[16],
         &data[17], &data[18], &data[19], &data[20], &data[21], &data[22], &data[23], &data[24],
         &data[25]);
  if (n_args < 25) {
    throw std::runtime_error("Unexpected data in GetRatingInformation: " + response);
  }

  // If something is commented out, then it means we aren't interested in these sensors (at the moment).
  // grid_rating_voltage = data[0] / 10.f;
  // grid_rating_current = data[1] / 10.f;
  // ac_output_rating_voltage = data[2] / 10.f;
  // ac_output_rating_frequency = data[3] / 10.f;
  // ac_output_rating_current  = data[4] / 10.f;
  // ac_output_rating_apparent_power = data[5];
  // ac_output_rating_active_power = data[6];
  const auto battery_nominal_voltage = data[7] / 10.f;
  battery_nominal_voltage_.Update(battery_nominal_voltage);
  battery_stop_discharging_voltage_with_grid_.Update(data[8] / 10.f);  // battery_recharge_voltage

  SetBatteryStopChargingVoltageWithGrid(battery_nominal_voltage, data[9]); // redischarge_voltage
  battery_under_voltage_.Update(data[10] / 10.f);
  battery_bulk_voltage_.Update(data[11] / 10.f);
  battery_float_voltage_.Update(data[12] / 10.f);
  battery_type_.Update(GetBatteryType(data[13]));
  // max_ac_charging_current = data[14];
  // max_charging_current = data[15];
  input_voltage_range_.Update(GetInputVoltageRange(data[16]));
  output_source_priority_.Update(GetOutputSourcePriority(data[17]));
  charger_source_priority_.Update(GetChargerPriority(data[18]));
  // parallel_max_num = data[19];
  machine_type_.Update(GetMachineType(data[20]));
  // topology = GetTopology(data[21]);
  // output_mode = GetOutputMode(data[22]); Single module, parallel output, Phase 1 of three phase...
  solar_power_priority_.Update(GetSolarPowerPriority(data[23]));
  // (Unused) data[24] - MPPT string
  // (Unused) data[25] - ??? There is no such param according to the protocol, but my inverter
  // returns it.
}

static std::string_view GetFaultCodeDescription(int code) {
  switch (code) {
    case 1: return "Fan is locked";
    case 2: return "Over temperature";
    case 3: return "Battery voltage is too high";
    case 4: return "Battery voltage is too low";
    case 5: return "Output short circuited or Over temperature";
    case 6: return "Output voltage is too high";
    case 7: return "Over load time out";
    case 8: return "Bus voltage is too high";
    case 9: return "Bus soft start failed";
    case 11: return "Main relay failed";
    case 51: return "Over current inverter";
    case 52: return "Bus soft start failed";
    case 53: return "Inverter soft start failed";
    case 54: return "Self-test failed";
    case 55: return "Over DC voltage on output of inverter";
    case 56: return "Battery connection is open";
    case 57: return "Current sensor failed";
    case 58: return "Output voltage is too low";
    case 60: return "Inverter negative power";
    case 71: return "Parallel version different";
    case 72: return "Output circuit failed";
    case 80: return "CAN communication failed";
    case 81: return "Parallel host line lost";
    case 82: return "Parallel synchronized signal lost";
    case 83: return "Parallel battery voltage detect different";
    case 84: return "Parallel Line voltage or frequency detect different";
    case 85: return "Parallel Line input current unbalanced";
    case 86: return "Parallel output setting different";
    default: throw std::runtime_error(std::format("Unknown fault code: {}", code));
  }
}

void Pi18ProtocolAdapter::GetWarnings() {
  // Special case. According to the protocol, the length is 34 (probably an error, should be 37).
  // But my inverter returns 39.
  // Therefore, I can't check it as a prefix and have to skip it here.
  auto str = GetFaultAndWarningStatusRaw().substr(2);

  // Response according to the protocol:
  // AA,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q
  // But my inverter for some reason returns an extra argument at the end of the format.
  int data[17];
  const auto n_args = sscanf(str.c_str(),
                             "%2d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d",
                             &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8],
                             &data[9], &data[10], &data[11], &data[12], &data[13], &data[14], &data[15], &data[16]);
  if (n_args < 17) {
    throw std::runtime_error("Unexpected data in GetRatingInformation: " + str);
  }
  std::vector<std::string> result;
  if (data[0] != 0) {
    result.emplace_back(GetFaultCodeDescription(data[0]));
  }
  if (data[1]) result.emplace_back("Line fail");
  if (data[2]) result.emplace_back("Output circuit short");
  if (data[3]) result.emplace_back("Inverter over temperature");
  if (data[4]) result.emplace_back("Fan lock");
  if (data[5]) result.emplace_back("Battery voltage high");
  if (data[6]) result.emplace_back("Battery low");
  if (data[7]) result.emplace_back("Battery under");
  if (data[8]) result.emplace_back("Over load");
  if (data[9]) result.emplace_back("Eeprom fail");
  if (data[10]) result.emplace_back("Power limit");
  if (data[11]) result.emplace_back("PV1 voltage high");
  if (data[12]) result.emplace_back("PV2 voltage high");
  if (data[13]) result.emplace_back("MPPT1 overload warning");
  if (data[14]) result.emplace_back("MPPT2 overload warning");
  if (data[15]) result.emplace_back("Battery too low to charge for SCC1");
  if (data[16]) result.emplace_back("Battery too low to charge for SCC2");

  warnings_.Update(Concatenate(result, '\n'));
}

void Pi18ProtocolAdapter::GetStatusInfo() {
  auto str = GetGeneralStatusRaw();

  // Response according to the protocol:
  // Response: AAAA,BBB,CCCC,DDD,EEEE,FFFF,GGG,HHH,III,JJJ,KKK,LLL,MMM,NNN,OOO,PPP,QQQQ,RRRR,SSSS,TTTT,U,V,W,X,Y,Z,a,b
  int data[28];
  const auto n_args = sscanf(str.c_str(),
                             "%4d,%3d,%4d,%3d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%4d,%4d,%4d,%4d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d",
                             &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8],
                             &data[9], &data[10], &data[11], &data[12], &data[13], &data[14], &data[15], &data[16],
                             &data[17], &data[18], &data[19], &data[20], &data[21], &data[22], &data[23], &data[24],
                             &data[25], &data[26], &data[27]);
  if (n_args < std::size(data)) {
    throw std::runtime_error("Unexpected data in GetGeneralInfo: " + str);
  }

  grid_voltage_.Update(data[0] / 10.f);
  grid_frequency_.Update(data[1] / 10.f);
  ac_output_voltage_.Update(data[2] / 10.f);
  ac_output_frequency_.Update(data[3] / 10.f);
  ac_output_apparent_power_.Update(data[4]);
  ac_output_active_power_.Update(data[5]);
  output_load_percent_.Update(data[6]);

  battery_voltage_.Update(data[7] / 10.f);
  battery_voltage_from_scc_.Update(data[8] / 10.f);
  battery_voltage_from_scc2_.Update(data[9] / 10.f);
  battery_discharge_current_.Update(data[10]);
  battery_charging_current_.Update(data[11]);
  battery_capacity_.Update(data[12]);
  inverter_heat_sink_temperature_.Update(data[13]);
  mptt1_charger_temperature_.Update(data[14]);
  mptt2_charger_temperature_.Update(data[15]);
  pv_input_power_.Update(data[16]);
  pv2_input_power_.Update(data[17]);
  pv_input_voltage_.Update(data[18] / 10.f);
  pv2_input_voltage_.Update(data[19] / 10.f);
  // data[20] - Setting value configuration state (0: Nothing changed, 1: Something changed)
  // data[21] - MPPT1 charger status (0: abnormal, 1: normal but not charged, 2: charging)
  // data[22] - MPPT2 charger status (0: abnormal, 1: normal but not charged, 2: charging)
  load_connection_.Update(data[23]);  // Load connection (0: disconnect, 1: connect)
  // data[24] - Battery power direction (0: donothing, 1: charge, 2: discharge)
  // data[25] - DC/AC power direction (0: donothing, 1: AC-DC, 2: DC-AC)
  // data[26] - Line power direction (0: donothing, 1: input, 2: output)
  // data[27] - Local parallel ID (a: 0~(parallel number - 1))

  // Other status info.
  // TODO: Total generated energy is temporarily disabled since at some point the inverter starts
  //  sending rubbish with incorrect CRC.
  // GetTotalGeneratedEnergy();
  mode_.Update(GetDeviceMode(GetWorkingModeRaw()));
  GetFlagsStatus();
  GetWarnings();
}

void Pi18ProtocolAdapter::GetFlagsStatus() {
  // Response: ^D020A,B,C,D,E,F,G,H,I<CRC><cr>
  auto response = Query("^P007FLAG", "^D020");
  int data[9];
  const auto n_args = sscanf(
      response.c_str(), "%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d",
      &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8]);
  if (n_args < std::size(data)) {
    throw std::runtime_error("Unexpected data in GetGeneralInfo: " + response);
  }
  // data[0] - Enable/disable silence buzzer or open buzzer
  // data[1] - Enable/Disable overload bypass function
  // data[2] - Enable/Disable LCD display escape to default page after 1min timeout
  // data[3] - Enable/Disable overload restart
  // data[4] - Enable/Disable over temperature restart
  backlight_.Update(data[5]);  // Enable/Disable backlight on
  // data[6] - Enable/Disable alarm on when primary source interrupt
  // data[7] - Enable/Disable fault code record
  // data[8] - Reserved
}

void Pi18ProtocolAdapter::GetTotalGeneratedEnergy() {
  // Response: NNNNNNNN, unit: KWh
  auto str = GetTotalGeneratedEnergyRaw();
  int result;
  if(sscanf(str.c_str(), "%8d", &result) != 1) {
    throw std::runtime_error("Unexpected data in GetTotalGeneratedEnergy: " + str);
  }
  total_energy_.Update(result);
}

bool Pi18ProtocolAdapter::SendCommand(std::string_view command) {
  constexpr auto kCommandAccepted = "^1";
  try {
    Query(command, kCommandAccepted);
    return true;
  } catch (const UnexpectedResponseException&) {
    return false;
  }
}

bool Pi18ProtocolAdapter::SetChargerPriority(ChargerPriority p) {
  return SendCommand(std::format("^S009PCP0,{}", GetChargerPriority(p)));
}

bool Pi18ProtocolAdapter::SetOutputSourcePriority(OutputSourcePriority p) {
  return SendCommand(std::format("^S007POP{}", GetOutputSourcePriority(p)));
}

bool Pi18ProtocolAdapter::SetBatteryType(BatteryType t) {
  return SendCommand(std::format("^S007PBT{}", GetBatteryType(t)));
}

bool Pi18ProtocolAdapter::SetInputVoltageRange(InputVoltageRange r) {
  return SendCommand(std::format("^S007PGR{}", GetInputVoltageRange(r)));
}

bool Pi18ProtocolAdapter::SetSolarPowerPriority(SolarPowerPriority p) {
  return SendCommand(std::format("^S007PSP{}", GetSolarPowerPriority(p)));
}

bool Pi18ProtocolAdapter::TurnBacklight(bool state) {
  const std::string_view flag = state ? "E" : "D";
  return SendCommand(std::format("^S006P{}F", flag));
}

bool Pi18ProtocolAdapter::TurnLoadConnection(bool on) {
  const std::string_view flag = on ? "1" : "0";
  return SendCommand(std::format("^S007LON{}", flag));
}
