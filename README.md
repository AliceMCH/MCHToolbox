# Muon Tracking commissioning scripts

## Run Type configuration

The type of data taking configuration can be configured with the following command:
```
./set-run-type.sh [0-9]
```
where the numerical parameter is an integer from 0 to 9, with the following meanings:
```
0: pedestals
1: pedestals_ul
2: physics_triggered
3: physics_triggered_csum
4: physics_triggered_ul
5: physics_triggered_ul_csum
6: physics_continuous_ul
7: physics_continuous_ul_csum
8: hb_check
9: ber_check
```

Each time the run type is changed the CRUs need to be re-configured (see next section).

## CRU configuration

The CRU parameters that control the front-end configuration and data taking
are defined in the `cru-init-env.sh` script.
The most important parameters that can be set are the following:

* `CRU_CLKSRC`: can be `local` (clock is generated internally) or `ttc` (clock comes from the LTU). In the latter case the data taking can only be controlled via the LTU GUI.
* `NSAMPLES`: number of ADC samples per event in pedestal mode (`RUNTYPE=pedestals`)
* `NSAMPLES_UL`: number of ADC samples per event in pedestal mode (`RUNTYPE=pedestals_ul`)

The type of data taking configuration must be set using the `./set-run-type.sh` script before invoking `./cru-init-env.sh`.

Each time some parameter is changed, the environment of each CRU needs to be updated:
```
./cru-init-env.sh CRUID
```
This will generate an `env-CRUID.sh` file in the scripts folder, which contains the configuration specific to the given CRU.

The CRU hardware can then be configured according to the newly created `env-CRUID.sh` file with this command:
```
./cru-config.sh CRUID
```

Notice that it is possible to have different CRUs with different enviroments on the same FLP (for example one CRU with local clock and the others with TTC clock).
After the `./cru-init-env.sh` file is modified, the configuration for a specific CRU is only updated by running `./cru-init-env.sh CRUID`. Otherwise the latest `env-CRUID.sh` file will be used.

### Updating all CRUs in one FLP

In order to update the configutation of all CRUs in one FLP, the following script is provided
```
./flp-init-env.sh
```

Afterwards it is possible to re-configure the CRU hardware like this
```
./flp-config.sh
```
This command will open one terminal window for each CRU being configured, and the configuration will proceed in parallel.
The terminal windows will stay open aftern the configuration is completed, allowing to eventually check for error messages, and they need to be closed explicitly.

The valid CRU IDs for each FLP can be found in the `flp.map` file.

## SOLAR configuration

Once the CRUs are initialized, one can proceed to configuring the SOLAR and DualSAMPA boards.

The SOLAR boards can be configured with the following commands:
```
./cru-solar-config.sh CRUID
```
or
```
./flp-solar-config.sh
```
If any of the SOLAR boards is not UP, a message will be displayed asking to power-cycle the corresponding crate. After pressing the ENTER key the links will be checked again, and the script will proceed to the actual configuration once all SOLARs are correctly UP. In some cases it might take two or more power-cycles in order to get all boards UP.

## DualSAMPA configuration

Once the SOLAR boards are configured, the last step is to program the registers of of the DualSAMPA boards:
```
./cru-ds-config.sh CRUID
```
or
```
./flp-ds-config.sh
```

The number of ADC samples per event in pedestal mode is defined in the CRU initialization script `cru-init-env.sh`

## Full re-configuration

The following commands allow to run the three configuration steps (CRU, SOLAR and DualSAMPA) at once:
```
./cru-config-all.sh CRUID
```
or
```
./flp-config-all.sh
```
The commands will stop as soon as an error is encountered.

# Trigger configuration

The patplayer can be configured according to the chosen run type with the following scripts:
```
./cru-config-trigger.sh CRUID
```
or
```
./flp-config-trigger.sh CRUID
```
to configure all the CRUs in the FLP at once.

This script are currently needed also when setting-up the CRUs for taking data with AliECS.

# Data taking

Data from a single CRU can be collected for both clock sources. The data taking is handled via the following script:
```
./cru-start-daq.sh CRUID
```
When `CRU_CLKSRC=local` two windows will appear. The top one shows the messages from the readout process, while the smaller one at the bottom allows to start and stop the data taking. The DAQ is started by hitting the ENTER key in the small window, and stopped by hitting ENTER again.

To close the readout window one has to click in it and then hit Ctrl+c.

The data is writte to the following file:
```
$HOME/Data/data-CRUID.raw
```
(where one has to replace `CRUID` by the actual ID of the CRU in use).

An helper script allows to check the quality of the data and print the errors:
```
./cru-check-data.sh CRUID
```
