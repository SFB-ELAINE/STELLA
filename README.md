# The SofTware dEfined impLantabLe modular plAtform (STELLA) for preclinical deep brain stimulation

We aimed to develop an innovative neurostimulator that significantly increases both animal welfare and the scientific value of DBS studies. 

Further details can be found at:

Franz Plocksties, Maria Kober, Christoph Niemann, Jakob Heller, Mareike Fauser, Martin Nüssel, Felix Uster, Denise Franz, Monique Zwar, Anika Lüttig, Justin Kröger, Jörg Harloff, Axel Schulz, Angelika Richter, Rüdiger Köhling, Dirk Timmermann, Alexander Storch:
The software defined implantable modular platform (STELLA) for preclinical deep brain stimulation research in rodents
In Journal of Neural Engineering, Vol. 18, Iss. 5, ISSN: 1741-2560, pp. 1-20, DOI: 10.1088/1741-2552/ac23e1, September 2021

[STELLA MAIN PCB](STELLA/tree/main/STELLA PCB)

To link STELLA to the outside world, we have developed an energy-efficient RF Shield that operates in the ISM band. This shield can be piggybacked by STELLA achieving a compact implant. 

Further details can be found at:

Franz Plocksties, Obaid Ullah Shah, Felix Uster, Munawar Ali, Maximilian Koschay, Maria Kober, Alexander Storch, Dirk Timmermann:
Energy-efficient modular RF interface for fully implantable electrical devices in small rodents
In Proceedings of the IEEE Biomedical Circuits and Systems Conference (BioCAS), pp. 1-6, DOI:10.1109/BioCAS49922.2021.9645022, Berlin, Deutschland, Oktober 2021

[STELLA RF SHIELD](STELLA/tree/main/STELLA RF SHIELD)


## Comparison of existing neurostimulators with STELLA

https://iopscience.iop.org/article/10.1088/1741-2552/ac23e1


|                             | Ewing (2013) [56] | Kouzani (2017) [27] | Kölbl (2016) [26] | Fluri (2017) [24] | Pinnell (2018) [29] | Adams (2019) [21] | Harnack (2008) [32] | Fleischer (2020) [57] | de Haas (2012) [31] | Hentall (2013) [33] | Millard (2007) [34]                          | Forni (2012) [25] | Alpaugh (2019) [30] | Liu (2017) [28] | Arfin (2009) [22] | STELLA                                         |
|-----------------------------|-------------------|---------------------|-------------------|-------------------|---------------------|-------------------|---------------------|-----------------------|---------------------|---------------------|----------------------------------------------|-------------------|---------------------|-----------------|-------------------|------------------------------------------------|
| Current amplitude (µA)      | 13-1000           | 200                 | 26-2036           | 10-500            | 20-2000             | -625              | 50-600              | up to 300             | 20-100              | 20-100              | 100-500                                      | 50-120            | 149                 | 200-500         | 10-1000           | 42-100                                         |
| Pulse-width (µs)            | 1µs-100% DC‡      | 90                  | 0.5µs-100% DC‡    | 60-500            | 10µs-100% DC‡       | 20 (min)          | 52                  | 60-500                | 60                  | 100-1000            | 25-250                                       | 0-80              | 103/111             | 30-1400         | 180               | 1µs-100% DC‡                                   |
| Frequency (Hz)              | 2-500             | 130                 | 10-300            | 10-200            | 0.1-5000            | 5000 (max)        | 131                 | 10-500                | 131                 | 8, 16 or 24         | 50-5000                                      | 0-130             | 130                 | 1-200           | 1400 (max)        | 10-5000                                        |
| Inter-pulse delay (µs)      | n/a               | n/a                 | 60                | n/a               | 0-pulse period      | n/a               | n/a                 | n/a                   | 200                 | n/a                 | n/a                                          | n/a               | 103                 | n/a             | n/a               | n/a                                            |
| Compliance voltage (V)      | 20                | 3 (battery)         | 17                | 3.1 (battery)     | 12                  | 10                | 18                  | 3.1 (battery)         | 4.65 (battery)      | 34                  | 5                                            | 6 (battery)       | 2,1                 | 3 (battery)     | 5                 | 2.2-2.8                                        |
| No.  of channels            | 2                 | 1                   | 1                 | 1                 | 2                   | 1                 | 1                   | 1                     | 2                   | 1                   | 1                                            | 1                 | 1                   | 1               | 4                 | 2                                              |
| Charge balancing            | Passive           | Passive             | Active            | Passive           | Active/Passive      | Active            | Active              | Passive               | Active              | Passive             | Active                                       | n/a               | Active              | Passive         | Active            | Passive                                        |
| Lifetime (DBS-on)           | 10 days           | 22 days             | 6 days            | 30 days           | 30 hours            | 9 hours           | 28 days             | 30 days               | 10 hours            | n/a (rests)         | ∞                                            | 7                 | 30 days             | 3 months        | 12 days           | 126 days (CR1216), 241 days (CR1225) ¤         |
| Type of battery             | ICP521630PM       | CR2032              | 3xp675            | 2xV364            | CR1225              | CR2032            | 2xSR48              | 2xV364                | 3xSR416             | 2xSR41              | none, energy provided through inductive cage | 2xCR1220          | 2xSR48              | CR2032          | 2xML614           | CR1216, CR1220, CR1225, CR2032□                |
| Battery weight (g)          | 5,4               | 3                   | 3x1.8             | 2x0.3             | 0.9                 | 3                 | 2x1.1               | 2x0.3                 | 3x0.13              | 2x0.6               | -                                            | 2x0.8             | 2x1.1               | 3               | 2x0.16            | 0.6 (CR1216), 0.9 (CR1225) □                   |
| PCB weight (g)              | n/a               | 0.92                | n/a               | n/a               | 0.8                 | Feb 18            | n/a                 | n/a                   | n/a                 | n/a                 | n/a                                          | n/a               | n/a                 | n/a             | n/a               | 0.6                                            |
| Total stimulator weight (g) | 11.5¶             | 4.75§               | 13.8¶             | 2.5§              | 2.8¶                | 5§                | 13¶                 | 2.8§                  | 2.1¶                | 2¶                  | 2.5{                                         | 5.6{/7.4¶         | 4.7¶                | 6§/16¶          | 1.3§              | 2.6 (CR1216),                    4 (CR1225)¶ □ |
| Battery volume (mm3)        | 3052              | 1000                | 3x550             | 2x70              | 300                 | 1000              | 2x260               | 2x70                  | 3x30                | 2x160               | -                                            | 2x250             | 2x260               | 1000            | 2x50              | 200 (CR1216),300 (CR1225) □                    |
| PCB size (mm)               | n/a               | 15x18x3*            | 26x12x3*          | n/a               | Ø12.5x5             | 30x25x3*          | 35x17.1x3*          | 10x10-1               | 25x8x3*             | n/a                 | n/a                                          | 28x9x3*           | n/a                 | Ø20x4           | 13x13x3*          | Ø13x3.3                                        |
| PCB volume (mm3)            | 2228              | 810                 | 936               | 1477              | 613                 | 2250              | 1796                | 130                   | 600                 | 688                 | n/a                                          | 756               | n/a                 | 1256            | 507               | 438                                            |
| PCB + battery volume (mm3)  | 5280              | 1810                | 2586              | 1617              | 913                 | 3250              | 2316                | 270                   | 690                 | 1008                | -                                            | 1256              | n/a                 | 2256            | 607               | 638 (CR1216),               738 (CR1225) □     |
| Total stimulator size (mm)  | 33x20x8¶          | 15x18x10*§          | 30x20x24*¶        | 21x11x7§          | Ø19.2x12.4¶         | 32.5x28x8§        | 38.5x20x13.5¶       | n/a                   | Ø8x30¶              | 18x8x7¶             | 14x12x6{                                     | 15x28x7¶          | n/a                 | Ø20x8¶          | n/a               | Ø15.5×7.5 (CR1216), Ø17.0×9.0 (CR1225)¶ □      |
| Control medium              | Magnet            | Pushbutton          | n/a               | Jumper            | Magnet              | Magnet            | Electromagn.        | Magnet                | Magnet              | Magnet              | Electromagn.                                 | n/a               | n/a                 | Electromagn.    | Electromagn.      | Magnet◆                                        |
| Output medium               | Visual LED        | Visual LED          | n/a               | Visual LED        | Visual LED          | Visual LED        | Electromagn.        | Visual LED            | IR LED              | IR LED              | Visual LED                                   | n/a               | Visual LED          | Electromagn.    | Visual LED        | Visual LED◆                                    |
| Status readout              | ♣                 | Programming status  | n/a               | On/Off/♠          | On/Off              | On/Off/♣          | Program var.        | On/Off                | On/Off/♠            | Program var.        | On/Off                                       | n/a               | ♣                   | On/Off/♣♠       | On/Off            | On/Off/♣♠◆                                     |
| Mounting                    | Head or Back      | Back                | Head              | Head              | Head                | Back              | Implant             | Implant               | Head + Implant      | Implant             | Implant + cage                               | Head              | Implant             | Head            | Head              | Implant                                        |
| Open source†                | No                | No                  | No                | No                | Complete            | No                | No                  | No                    | No                  | No                  | Incomplete                                   | No                | No                  | No              | Incomplete        | Complete                                       |

*Estimated from photographic scale <br />
‡Duty Cycle <br />
Including §battery, {housing, ¶both <br />
†Including all design/layout/firmware files and datasets for reproduction <br />
¤ Refers to the stimulation parameters (1x100 µA, 130 Hz, 60 µs) for LPM3 at a boost voltage of 3.1 V  <br />
□ Highly flexible regarding needed stimulation time and rodent’s size and weight <br />
♣Battery voltage / ♠load impedance out of range indication <br />
◆ Extendable by inter-module communication <br />


## License

This project is licensed under the terms of GNU General Public License v3.0