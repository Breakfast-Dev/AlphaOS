#include "pci.h"
#include "ahci/ahci.h" // For prevent infinite reference error

namespace PCI
{
    void EnumrateFunction(uint64_t deviceAddress, uint64_t function)
    {
        uint64_t offset = function << 12;
        uint64_t functionAddress = deviceAddress + offset;

        g_PageTableManager.MapMemory((void *)functionAddress, (void *)functionAddress);

        PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)functionAddress;

        if (pciDeviceHeader->DeviceID == 0)
            return;
        if (pciDeviceHeader->DeviceID == 0xFFFF)
            return;

        // GlobalRenderer->Print("PCI Device: ");

        // GlobalRenderer->Print(GetVendorName(pciDeviceHeader->VendorID));
        // GlobalRenderer->Print(" | ");

        // GlobalRenderer->Print(GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
        // GlobalRenderer->Print(" | ");

        // GlobalRenderer->Print(PCI::DeviceClasses[pciDeviceHeader->Class]);
        // GlobalRenderer->Print(" | ");

        // GlobalRenderer->Print(GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->Subclass));
        // GlobalRenderer->Print(" | ");

        // GlobalRenderer->Print(GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->Subclass, pciDeviceHeader->ProgIF));

        // GlobalRenderer->Next(1);

        switch (pciDeviceHeader->Class)
        {
        case 0x01:
        {
            switch (pciDeviceHeader->Subclass)
            {
            case 0x06:
            {
                switch (pciDeviceHeader->ProgIF)
                {
                case 0x01:
                {
                    new AHCI::AHCIDriver(pciDeviceHeader);
                }
                }
            }
            }
        }
        }

        return;
    }

    void EnumrateDevice(uint64_t busAddress, uint64_t device)
    {
        uint64_t offset = device << 15;
        uint64_t deviceAddress = busAddress + offset;

        g_PageTableManager.MapMemory((void *)deviceAddress, (void *)deviceAddress);

        PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)deviceAddress;

        if (pciDeviceHeader->DeviceID == 0)
            return;
        if (pciDeviceHeader->DeviceID == 0xFFFF)
            return;

        for (uint64_t function = 0; function < 8; function++)
        {
            EnumrateFunction(deviceAddress, function);
        }

        return;
    }

    void EnumrateBus(uint64_t baseAddress, uint64_t bus)
    {
        uint64_t offset = bus << 20;
        uint64_t busAddress = baseAddress + offset;

        g_PageTableManager.MapMemory((void *)busAddress, (void *)busAddress);

        PCIDeviceHeader *pciDeviceHeader = (PCIDeviceHeader *)busAddress;

        if (pciDeviceHeader->DeviceID == 0)
            return;
        if (pciDeviceHeader->DeviceID == 0xFFFF)
            return;

        for (uint64_t device = 0; device < 32; device++)
        {
            EnumrateDevice(busAddress, device);
        }

        return;
    }

    void EnumratePCI(ACPI::MCFGHeader *mcfg)
    {
        int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);

        for (int t = 0; t < entries; t++)
        {
            ACPI::DeviceConfig *newDeviceConfig = (ACPI::DeviceConfig *)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));

            for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++)
            {
                EnumrateBus(newDeviceConfig->BaseAddress, bus);
            }
        }

        return;
    }
}