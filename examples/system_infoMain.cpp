// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <fmt/core.h>
#include <hwinfo/hwinfo.h>
#include <hwinfo/utils/unit.h>

#include <cxxopts.hpp>

#include <string>
#include <cassert>
#include <vector>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using hwinfo::unit::bytes_to_MiB;


class HardwareDetectScope
{
 public:
   HardwareDetectScope(const std::string& scope_string)
   {
     detect_cpu = false;
     detect_os = false;
     detect_gpu = false;
     detect_memory = false;
     detect_main_board = false;
     detect_battery = false;
     detect_disks = false;
     detect_network = false;
     detect_all = false;

     if (scope_string.empty()) {
       return;
     }

     std::stringstream ss(scope_string);
     std::string item;

     while (std::getline(ss, item, ',')) {
       item.erase(0, item.find_first_not_of(" \t"));
       item.erase(item.find_last_not_of(" \t") + 1);

       if (item == "all") {
         detect_all = true;
         detect_cpu = true;
         detect_os = true;
         detect_gpu = true;
         detect_memory = true;
         detect_main_board = true;
         detect_battery = true;
         detect_disks = true;
         detect_network = true;
         return;
       }
       else if (item == "cpu") {
         detect_cpu = true;
       } else if (item == "os") {
         detect_os = true;
       } else if (item == "gpu") {
         detect_gpu = true;
       } else if (item == "memory") {
         detect_memory = true;
       } else if (item == "main_board") {
         detect_main_board = true;
       } else if (item == "battery") {
         detect_battery = true;
       } else if (item == "disks") {
         detect_disks = true;
       } else if (item == "network") {
         detect_network = true;
       }
     }
   }

 public:
  bool detect_cpu;
  bool detect_os;
  bool detect_gpu;
  bool detect_memory;
  bool detect_main_board;
  bool detect_battery;
  bool detect_disks;
  bool detect_network;
  bool detect_all;
};


void showRawInfo()
{
  fmt::print(
      "hwinfo is an open source, MIT licensed project that implements a platform independent "
      "hardware and system information gathering API for C++.\n\n"
      "If you face any issues, find bugs or if your platform is not supported yet, do not hesitate to create "
      "a ticket at https://github.com/lfreist/hwinfo/issues.\n\n"
      "Thanks for using hwinfo!\n\n");

  fmt::print(
      "Hardware Report:\n\n"
      "----------------------------------- CPU ------------------------------------\n");
  const auto cpus = hwinfo::getAllCPUs();
  for (const auto& cpu : cpus) {
    // clang-format off
    fmt::print("Socket {}:\n"
               "{:<20} {}\n"
               "{:<20} {}\n"
               "{:<20} {}\n"
               "{:<20} {}\n"
               "{:<20} {}\n"
               "{:<20} {}\n"
               "{:<20} L1: {}, L2: {}, L3: {}\n",
               cpu.id(),
               "vendor:", cpu.vendor(),
               "model:", cpu.modelName(),
               "physical cores:", cpu.numPhysicalCores(),
               "logical cores:", cpu.numLogicalCores(),
               "max frequency:", cpu.maxClockSpeed_MHz(),
               "regular frequency:", cpu.regularClockSpeed_MHz(),
               "cache size:", cpu.L1CacheSize_Bytes(), cpu.L2CacheSize_Bytes(), cpu.L3CacheSize_Bytes());
    // clang-format on

    std::vector<double> threads_utility = cpu.threadsUtilisation();
    std::vector<int64_t> threads_speed = cpu.currentClockSpeed_MHz();
    assert((threads_utility.size() == threads_speed.size()));
    for (size_t thread_id = 0; thread_id != threads_utility.size(); ++thread_id) {
      fmt::print("{:<20} Thread {}: {} MHz ({}%)\n", " ", thread_id, threads_speed[thread_id],
                 threads_utility[thread_id] * 100);
    }
    // fmt::print("{}\n", cpu.currentTemperature_Celsius());
  }

  hwinfo::OS os;
  // clang-format off
  fmt::print("----------------------------------- OS ------------------------------------\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n",
             "Operating System:", os.name(),
             "version:", os.version(),
             "kernel:", os.kernel(),
             "architecture:", os.is32bit() ? "32 bit" : "64 bit",
             "endianess:", os.isLittleEndian() ? "little endian" : "big endian");
  // clang-format on

  auto gpus = hwinfo::getAllGPUs();
  fmt::print("----------------------------------- GPU -----------------------------------\n");
  for (const auto& gpu : gpus) {
    // clang-format off
    fmt::print("GPU {}:\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n",
                gpu.id(),
                "vendor:", gpu.vendor(),
                "model:", gpu.name(),
                "driverVersion:", gpu.driverVersion(),
                "memory [MiB]:", bytes_to_MiB(gpu.memory_Bytes()),
                "frequency:", gpu.frequency_MHz(),
                "cores:", gpu.num_cores(),
                "vendor_id:", gpu.vendor_id(),
                "device_id:", gpu.device_id());
    // clang-format on
  }

  hwinfo::Memory memory;
  // clang-format off
  fmt::print("----------------------------------- RAM -----------------------------------\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n",
             "size [MiB]:", bytes_to_MiB(memory.total_Bytes()),
             "free [MiB]:", bytes_to_MiB(memory.free_Bytes()),
             "available [MiB]:", bytes_to_MiB(memory.available_Bytes()));
  // clang-format on

  for (const auto& module : memory.modules()) {
    // clang-format off
    fmt::print("RAM {}:\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n"
                "{:<20} {}\n",
                module.id,
                "vendor:", module.vendor,
                "model:", module.model,
                "name:", module.name,
                "serial-number:", module.serial_number,
                "Frequency [MHz]:", module.frequency_Hz == -1 ? -1 : static_cast<double>(module.frequency_Hz) / 1e6);
    // clang-format on
  }

  hwinfo::MainBoard main_board;
  // clang-format off
  fmt::print("------------------------------- Main Board --------------------------------\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n"
             "{:<20} {}\n",
             "vendor:", main_board.vendor(),
             "name:", main_board.name(),
             "version:", main_board.version(),
             "serial-number:", main_board.serialNumber());
  // clang-format on

  std::vector<hwinfo::Battery> batteries = hwinfo::getAllBatteries();

  fmt::print("------------------------------- Batteries ---------------------------------\n");
  if (!batteries.empty()) {
    int battery_counter = 0;
    for (auto& battery : batteries) {
      // clang-format off
      fmt::print("Battery {}:\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n",
                 battery_counter++,
                 "vendor:", battery.vendor(),
                 "model:", battery.model(),
                 "serial-number:", battery.serialNumber(),
                 "charging:", battery.charging() ? "yes" : "no",
                 "capacity:", battery.capacity());
      // clang-format on
    }
  } else {
    fmt::print("No Batteries installed or detected\n");
  }

  std::vector<hwinfo::Disk> disks = hwinfo::getAllDisks();
  fmt::print("--------------------------------- Disks -----------------------------------\n");
  if (!disks.empty()) {
    int disk_counter = 0;
    for (const auto& disk : disks) {
      // clang-format off
      fmt::print("Disk {}:\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n"
                 "{:<20} {}\n",
                 disk_counter++,
                 "vendor:", disk.vendor(),
                 "model:", disk.model(),
                 "serial-number:", disk.serialNumber(),
                 "size:", disk.size_Bytes());
      // clang-format on
    }
  } else {
    fmt::print("No Disks installed or detected\n");
  }

  std::vector<hwinfo::Network> networks = hwinfo::getAllNetworks();
  fmt::print("--------------------------------- Networks -----------------------------------\n");
  if (!networks.empty()) {
    int network_counter = 0;
    for (const auto& network : networks) {
      // clang-format off
      if (network.ip4().size() > 0 || network.ip6().size() > 0) {
        fmt::print(
            "Network {}:\n"
            "{:<20} {}\n"
            "{:<20} {}\n"
            "{:<20} {}\n"
            "{:<20} {}\n"
            "{:<20} {}\n",
            network_counter++,
            "description:", network.description(),
            "interface index:", network.interfaceIndex(),
            "mac:", network.mac(),
            "ipv4:", network.ip4(),
            "ipv6:", network.ip6());
      }
      // clang-format on
    }
  } else {
    fmt::print("No Networks installed or detected\n");
  }
}


void showJsonInfo(HardwareDetectScope &scope, bool prettyPrint)
{
  json systemInfo = json({});

  if (scope.detect_cpu)
  {
    auto cpuInfoArray = json::array();
    const auto cpus = hwinfo::getAllCPUs();
    for (const auto& cpu : cpus) {
      json cpuInfo;

      cpuInfo["socket_id"] = cpu.id();
      cpuInfo["vendor"] = cpu.vendor();
      cpuInfo["model"] = cpu.modelName();
      cpuInfo["physical_cores"] = cpu.numPhysicalCores();
      cpuInfo["logical_cores"] = cpu.numLogicalCores();
      cpuInfo["max_frequency_mhz"] = cpu.maxClockSpeed_MHz();
      cpuInfo["regular_frequency_mhz"] = cpu.regularClockSpeed_MHz();

      json cacheInfo;
      cacheInfo["L1_bytes"] = cpu.L1CacheSize_Bytes();
      cacheInfo["L2_bytes"] = cpu.L2CacheSize_Bytes();
      cacheInfo["L3_bytes"] = cpu.L3CacheSize_Bytes();
      cpuInfo["cache"] = cacheInfo;

      cpuInfoArray.push_back(cpuInfo);
    }
    systemInfo["cpu"] = cpuInfoArray;
  }

  if (scope.detect_os)
  {
    hwinfo::OS os;
    json osInfo;
    osInfo["name"] = os.name();
    osInfo["version"] = os.version();
    osInfo["kernel"] = os.kernel();
    osInfo["architecture"] = os.is32bit() ? "32 bit" : "64 bit";
    osInfo["endianess"] = os.isLittleEndian() ? "little endian" : "big endian";
    systemInfo["os"] = osInfo;
  }

  if (scope.detect_gpu)
  {
    json gpuInfoArray = json::array();
    auto gpus = hwinfo::getAllGPUs();
    for (const auto& gpu : gpus) {
      json gpuInfo;

      gpuInfo["id"] = gpu.id();
      gpuInfo["vendor"] = gpu.vendor();
      gpuInfo["model"] = gpu.name();
      gpuInfo["driver_version"] = gpu.driverVersion();
      gpuInfo["memory_mib"] = bytes_to_MiB(gpu.memory_Bytes());
      gpuInfo["frequency_mhz"] = gpu.frequency_MHz();
      gpuInfo["cores"] = gpu.num_cores();
      gpuInfo["vendor_id"] = gpu.vendor_id();
      gpuInfo["device_id"] = gpu.device_id();

      gpuInfoArray.push_back(gpuInfo);
    }
    systemInfo["gpu"] = gpuInfoArray;
  }

  if (scope.detect_memory)
  {
    hwinfo::Memory memory;
    json memoryInfo;
    memoryInfo["total_mib"] = bytes_to_MiB(memory.total_Bytes());
    memoryInfo["free_mib"] = bytes_to_MiB(memory.free_Bytes());
    memoryInfo["available_mib"] = bytes_to_MiB(memory.available_Bytes());
    systemInfo["memory"] = memoryInfo;

    json memoryModulesArray = json::array();
    for (const auto& module : memory.modules()) {
      json memoryModuleInfo;

      memoryModuleInfo["id"] = module.id;
      memoryModuleInfo["vendor"] = module.vendor;
      memoryModuleInfo["model"] = module.model;
      memoryModuleInfo["name"] = module.name;
      memoryModuleInfo["serial-number"] = module.serial_number;
      memoryModuleInfo["frequency_mhz"] = module.frequency_Hz == -1 ? -1 : static_cast<double>(module.frequency_Hz) / 1e6;

      memoryModulesArray.push_back(memoryModuleInfo);
    }
    systemInfo["memory_modules"] = memoryModulesArray;

    hwinfo::MainBoard main_board;
    json mainBoardInfo;
    mainBoardInfo["vendor"] = main_board.vendor();
    mainBoardInfo["name"] = main_board.name();
    mainBoardInfo["version"] = main_board.version();
    mainBoardInfo["serial-number"] = main_board.serialNumber();
  }

  if (scope.detect_battery)
  {
    json batteryArray = json::array();
    std::vector<hwinfo::Battery> batteries = hwinfo::getAllBatteries();

    if (!batteries.empty()) {
      int battery_counter = 0;
      for (auto& battery : batteries) {
        json batteryInfo;

        batteryInfo["id"] = battery_counter++;
        batteryInfo["vendor"] = battery.vendor();
        batteryInfo["model"] = battery.model();
        batteryInfo["serial_number"] = battery.serialNumber();
        batteryInfo["charging"] = battery.charging();
        batteryInfo["capacity"] = battery.capacity();

        batteryArray.push_back(batteryInfo);
      }
    }

    systemInfo["batteries"] = batteryArray;
    systemInfo["battery_count"] = batteries.size();
    systemInfo["has_batteries"] = !batteries.empty();
  }

  if (scope.detect_disks)
  {
    json diskArray = json::array();
    std::vector<hwinfo::Disk> disks = hwinfo::getAllDisks();
    if (!disks.empty()) {
      int disk_counter = 0;
      for (const auto& disk : disks) {
        json diskInfo;

        diskInfo["id"] = disk_counter++;
        diskInfo["vendor"] = disk.vendor();
        diskInfo["model"] = disk.model();
        diskInfo["serial_number"] = disk.serialNumber();
        diskInfo["size_bytes"] = disk.size_Bytes();

        diskInfo["size_gb"] = static_cast<double>(disk.size_Bytes()) / (1024 * 1024 * 1024);

        diskArray.push_back(diskInfo);
      }
    }

    systemInfo["disks"] = diskArray;
    systemInfo["disk_count"] = disks.size();
    systemInfo["has_disks"] = !disks.empty();
  }

  if (scope.detect_network)
  {
    json networkArray = json::array();
    std::vector<hwinfo::Network> networks = hwinfo::getAllNetworks();
    if (!networks.empty()) {
      int network_counter = 0;
      for (const auto& network : networks) {
        if (network.ip4().size() > 0 || network.ip6().size() > 0) {
          json networkInfo;

          networkInfo["id"] = network_counter++;
          networkInfo["description"] = network.description();
          networkInfo["interface_index"] = network.interfaceIndex();
          networkInfo["mac"] = network.mac();
          networkInfo["ipv4"] = network.ip4();
          networkInfo["ipv6"] = network.ip6();

          networkArray.push_back(networkInfo);
        }
      }
    }

    systemInfo["networks"] = networkArray;
    systemInfo["network_count"] = networkArray.size();
    systemInfo["has_networks"] = !networkArray.empty();
  }

  if (prettyPrint)
	  fmt::print("{}\n", systemInfo.dump(4));
  else
      fmt::print("{}\n", systemInfo.dump());
}


int main(int argc, char** argv) {
  cxxopts::Options options("system_info", "Hardware information inspector.");
  options.add_options()
    ("f,format", "Output format (json, raw)", cxxopts::value<std::string>()->default_value("raw"))
    ("s,scope", "Hardware information scope (cpu, os, gpu, memory, main_board, battery, disks, network, all)", cxxopts::value<std::string>()->default_value("all"))
    ("p,pretty_print", "Pretty print json")
    ("h,help", "Print usage");
  auto arg_parse_result = options.parse(argc, argv);

  HardwareDetectScope scope(arg_parse_result["scope"].as<std::string>());

  if (arg_parse_result.count("help"))
  {
    fmt::print("{}\n",  options.help());
  } else if (!arg_parse_result.count("format") || arg_parse_result["format"].as<std::string>() == "raw")
  {
    showRawInfo();
  } else if (arg_parse_result["format"].as<std::string>() == "json") {
    showJsonInfo(scope, arg_parse_result.count("pretty_print") != 0);
  } else {
    fmt::print("{}\n",  options.help());
  }

  return EXIT_SUCCESS;
}
