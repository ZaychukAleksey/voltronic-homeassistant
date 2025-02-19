#pragma once

#include <format>

enum class DeviceMode : char {
  kPowerOn,             //  Power on mode
  kStandby,             //  Standby mode
  kBypass,              //  Bypass mode
  kBattery,             //  Battery mode
  kFault,               //  Fault mode
  kPowerSaving,         //  Power saving Mode
  kHybrid,              //  Hybrid mode(Line mode, Grid mode)
  kLine,                //  Line Mode
  kBatteryTest,         //  Battery test mode
  kShutdown,            //  Shutdown Mode
  kGrid,                //  Grid mode
  kCharge,              //  Charge mode
  kEco,                 //  ECO mode
};


inline void FromString(const std::string& str, DeviceMode& result) {
  if (str == "Power on") { result = DeviceMode::kPowerOn; return; }
  if (str == "Standby") { result = DeviceMode::kStandby; return; }
  if (str == "Bypass") { result = DeviceMode::kBypass; return; }
  if (str == "Battery") { result = DeviceMode::kBattery; return; }
  if (str == "Fault") { result = DeviceMode::kFault; return; }
  if (str == "Power saving") { result = DeviceMode::kPowerSaving; return; }
  if (str == "Hybrid") { result = DeviceMode::kHybrid; return; }
  if (str == "Line") { result = DeviceMode::kLine; return; }
  if (str == "Battery test") { result = DeviceMode::kBatteryTest; return; }
  if (str == "Shutdown") { result = DeviceMode::kShutdown; return; }
  if (str == "Grid") { result = DeviceMode::kGrid; return; }
  if (str == "Charge") { result = DeviceMode::kCharge; return; }
  if (str == "ECO") { result = DeviceMode::kEco; return; }
  throw std::runtime_error(std::format("Unexpected value for DeviceMode: {}", str));
}

constexpr std::string ToString(DeviceMode mode) {
  switch (mode) {
    case DeviceMode::kPowerOn: return "Power on";
    case DeviceMode::kStandby: return "Standby";
    case DeviceMode::kBypass: return "Bypass";
    case DeviceMode::kBattery: return "Battery";
    case DeviceMode::kFault: return "Fault";
    case DeviceMode::kPowerSaving: return "Power saving";
    case DeviceMode::kHybrid: return "Hybrid";
    case DeviceMode::kLine: return "Line";
    case DeviceMode::kBatteryTest: return "Battery test";
    case DeviceMode::kShutdown: return "Shutdown";
    case DeviceMode::kGrid: return "Grid";
    case DeviceMode::kCharge: return "Charge";
    case DeviceMode::kEco: return "ECO";
  }
  throw std::runtime_error(std::format("Unknown DeviceMode: {}", (int) mode));
}

enum class BatteryType : char {
  kAgm,             //  AGM
  kFlooded,         //  Flooded
  kUser,            //  User-defined
  kPYL,             //  Pylon (5048MG & 5048MGX Remote Panel Communication Protocol)
  kX,               //  X, (only for king)
  kWeco,            //  Weco; (only for king)
  kSol,             //  Sol, (only for king)
  kBak,             //  BAK(only for king)
  kSH,              //  SH (5048MG & 5048MGX Remote Panel Communication Protocol)
};

inline void FromString(const std::string& str, BatteryType& result) {
  if (str == "AGM") { result = BatteryType::kAgm; return; }
  if (str == "Flooded") { result = BatteryType::kFlooded; return; }
  if (str == "User-defined") { result = BatteryType::kUser; return; }
  if (str == "PYL") { result = BatteryType::kPYL; return; }
  if (str == "SH") { result = BatteryType::kSH; return; }
  throw std::runtime_error(std::format("Unexpected value for BatteryType: {}", str));
}

constexpr std::string ToString(BatteryType type) {
  switch (type) {
    case BatteryType::kAgm: return "AGM";
    case BatteryType::kFlooded: return "Flooded";
    case BatteryType::kUser: return "User-defined";
    case BatteryType::kPYL: return "PYL";
    case BatteryType::kSH: return "SH";
  }
  throw std::runtime_error(std::format("Unknown BatteryType: {}", (int) type));
}

enum class ChargerPriority : char {
  kUtilityFirst,    //  Utility first
  kSolarFirst,      //  Solar first
  kSolarAndUtility, //  Solar + Utility
  kOnlySolar,       //  Only solar charging permitted
};

inline void FromString(const std::string& str, ChargerPriority& result) {
  if (str == "Utility") { result = ChargerPriority::kUtilityFirst; return; }
  if (str == "Solar") { result = ChargerPriority::kSolarFirst; return; }
  if (str == "Solar+Utility") { result = ChargerPriority::kSolarAndUtility; return; }
  if (str == "Solar only") { result = ChargerPriority::kOnlySolar; return; }
  throw std::runtime_error(std::format("Unexpected value for ChargerPriority: {}", str));
}

constexpr std::string ToString(ChargerPriority priority) {
  switch (priority) {
    case ChargerPriority::kUtilityFirst: return "Utility";
    case ChargerPriority::kSolarFirst: return "Solar";
    case ChargerPriority::kSolarAndUtility: return "Solar+Utility";
    case ChargerPriority::kOnlySolar: return "Solar only";
  }
  throw std::runtime_error(std::format("Unknown ChargerPriority: {}", (int) priority));
}

enum class OutputSourcePriority : char {
  kUtility,                   //  Utility -> ??? -> ???
  kSolarUtilityBattery,       //  Solar -> Utility -> Battery
  kSolarBatteryUtility,       //  Solar -> Battery -> Utility
};

inline void FromString(const std::string& str, OutputSourcePriority& result) {
  if (str == "Utility") { result = OutputSourcePriority::kUtility; return; }
  if (str == "Solar->Utility->Battery") { result = OutputSourcePriority::kSolarUtilityBattery; return; }
  if (str == "Solar->Battery->Utility") { result = OutputSourcePriority::kSolarBatteryUtility; return; }
  throw std::runtime_error(std::format("Unexpected value for OutputSourcePriority: {}", str));
}

constexpr std::string ToString(OutputSourcePriority priority) {
  switch (priority) {
    case OutputSourcePriority::kUtility: return "Utility";
    case OutputSourcePriority::kSolarUtilityBattery: return "Solar->Utility->Battery";
    case OutputSourcePriority::kSolarBatteryUtility: return "Solar->Battery->Utility";
  }
  throw std::runtime_error(std::format("Unknown OutputSourcePriority: {}", (int) priority));
}

enum class OutputMode : char {
  kSingle,          //  single machine output
  kParallel,        //  parallel output
  kPhase1Of3,       //  Phase 1 of 3 Phase output
  kPhase2Of3,       //  Phase 2 of 3 Phase output
  kPhase3Of3,       //  Phase 3 of 3 Phase output
};

enum class MachineType : char {
  kGridTie,         //  Grid tie
  kOffGrid,         //  Off Grid
  kHybrid,          //  Hybrid
};

namespace impl_details {
constexpr auto kMachineTypeGridTie = "Grid-tie";
constexpr auto kMachineTypeOffGrid = "Off-grid";
constexpr auto kMachineTypeHybrid = "Hybrid";
}

constexpr std::string ToString(MachineType type) {
  switch (type) {
    case MachineType::kGridTie: return impl_details::kMachineTypeGridTie;
    case MachineType::kOffGrid: return impl_details::kMachineTypeOffGrid;
    case MachineType::kHybrid: return impl_details::kMachineTypeHybrid;
  }
  throw std::runtime_error(std::format("Unknown MachineType: {}", (int) type));
}

inline void FromString(const std::string& str, MachineType& result) {
  if (str == impl_details::kMachineTypeGridTie) { result = MachineType::kGridTie; return; }
  if (str == impl_details::kMachineTypeOffGrid) { result = MachineType::kOffGrid; return; }
  if (str == impl_details::kMachineTypeHybrid) { result = MachineType::kHybrid; return; }
  throw std::runtime_error(std::format("Unexpected value for MachineType: {}", str));
}


enum class Topology : char {
  kTransformless,   //  transformerless
  kTransformer,     //  transformer
};

enum class InputVoltageRange : char {
  kAppliance,       // Appliance
  kUps,             // UPS
};


namespace impl_details {
constexpr auto kAcVoltageRangeAppliance = "Appliance (90-280V)";
constexpr auto kAcVoltageRangeUps = "UPS (170-280V)";
}

constexpr std::string ToString(InputVoltageRange type) {
  switch (type) {
    case InputVoltageRange::kAppliance: return impl_details::kAcVoltageRangeAppliance;
    case InputVoltageRange::kUps: return impl_details::kAcVoltageRangeUps;
  }
  throw std::runtime_error(std::format("Unknown InputVoltageRange: {}", (int) type));
}

inline void FromString(const std::string& str, InputVoltageRange& result) {
  if (str == impl_details::kAcVoltageRangeAppliance) { result = InputVoltageRange::kAppliance; return; }
  if (str == impl_details::kAcVoltageRangeUps) { result = InputVoltageRange::kUps; return; }
  throw std::runtime_error(std::format("Unexpected value for InputVoltageRange: {}", str));
}

enum class SolarPowerPriority : char {
  kBatteryLoadUtility,
  kLoadBatteryUtility,
};

namespace impl_details {
constexpr auto kSpPriorityBLU = "Battery->Load->Utility";
constexpr auto kSpPriorityLBU = "Load->Battery->Utility";
}

constexpr std::string ToString(SolarPowerPriority type) {
  switch (type) {
    case SolarPowerPriority::kBatteryLoadUtility: return impl_details::kSpPriorityBLU;
    case SolarPowerPriority::kLoadBatteryUtility: return impl_details::kSpPriorityLBU;
  }
  throw std::runtime_error(std::format("Unknown SolarPowerPriority: {}", (int) type));
}

inline void FromString(const std::string& str, SolarPowerPriority& result) {
  if (str == impl_details::kSpPriorityBLU) { result = SolarPowerPriority::kBatteryLoadUtility; return; }
  if (str == impl_details::kSpPriorityLBU) { result = SolarPowerPriority::kLoadBatteryUtility; return; }
  throw std::runtime_error(std::format("Unexpected value for SolarPowerPriority: {}", str));
}
