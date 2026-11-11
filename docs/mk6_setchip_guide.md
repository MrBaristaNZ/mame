# Setting up Aristocrat MK6 games in MAME with the jurisdictional setchip

Most US MK6 sets need set-up memory before they will play. The set-up is done once per game with the setchip BIOS (`set-us1`), then the game runs on `us1`. Default keys below are the driver's (`aristmk6.cpp`). The key panel changes with the game type, so on-screen labels win over key names (see Notes).

Keys: Cashout `C`, Service `Y`, Bet 1 `Q`, Bet 2 `W`, Bet 3 `E`, Bet 5 `R`, Spin Again `Space`, Play 5 Lines `D`, Play 7 Lines `F`, Logic Door `Z`, Main Door `M`, Operator Key `O`, Jackpot Key `J`. Doors and keys are toggles.

## 1. Find the game's type first

Boot the game on `us1` with any save. Turn the Operator key on (`O`), wait about 8 s for the Operator Mode Menu, then choose item 1, Machine Identification (the "choose" button is shown on screen, usually `F`). The page shows a line such as `10 Credit Multiplier/3125-Way (50 bet) Multiway`. Look that line up in the table in section 3. This page needs no set-up. On a game with no valid set-up, the coin and credit values on it are defaults and can be ignored.

## 2. The process

1. Start with a fresh NVRAM folder and run `-bios set-us1`. First screen: "Game and Key Panel type". Press Cashout (`C`) to step to the next type. Hold Service (`Y`) while pressing Cashout to go back. Count from Multi-Line Game (0 presses) using the table. Check the type name on screen.
2. Hold Service (`Y`) and press Bet 5 (`R`) to clear SRAM. The type is then locked ("Restart Setchip to change Game Type").
3. **Version Menu: set Target Version to 6.03.04. Press Bet 1 (`Q`) three times (6.00.00 to 6.03.04), then Spin Again (`Space`).** This is not automatic. Left at 6.00.00, the jurisdiction list is different, the menu says "Playline Setup", and the game rejects the save with "EEPROM error! Run Setchip Program".
4. Jurisdiction: press Bet 1 (`Q`) to step the list. Under 6.03.04, Missouri is 40 presses from the start (gives $2 = 40 credits) and Nevada is 41. Count by reading the name on screen. Then Spin Again.
5. Main menu. Items: Version, Jurisdictional Options, Money Setup, Payout Setup, Gameplay Setup, Miscellaneous Options, Bill Validator Setup, Save All Options, Clear DLB EEPROM. Service (`Y`) goes to the next item, Cashout (`C`) to the previous, Bet 5 (`R`) enters. After the jurisdiction page, the cursor is on Jurisdictional Options (seen in step snapshots, gwnn, 2026-11-07).
6. Money Setup (Missouri only, to get $2 = 40 credits): enter, go to Value of one Coin (the cursor starts on Currency, so press Service once). Bet 1 (`Q`) raises it. $2.00 is 6 presses from $0.05. The screen says "$2.00 buys 40 credits". Spin Again returns. Nevada works at the $0.05 default (5c = 1 credit).
7. Gameplay Setup: only where the table gives lines or bets per line. Enter with Bet 5. Service moves between Number of Lines and Bets per Line. Bet 1 goes up, Bet 2 (`W`) goes down on the Reel Power and 50-line tests. Spin Again returns. The main menu shows the result, for example "243 Ways, 5 credits (x25) max".
8. Save All Options: select it, open the Logic Door (`Z`) if the screen says "YOU MUST OPEN THE LOGIC DOOR TO SAVE", then press Bet 5 (`R`). Wait for "JURISDICTIONAL MACHINE OPTIONS HAVE BEEN SAVED". Nothing is written before that message.
9. Quit MAME. The setchip NVRAM is in `nvram/<game>_41`. MAME keeps one folder per BIOS, and `us1` uses `_27`. Copy `nvram/<game>_41` to `nvram/<game>_27`. The settings live in `eeprom0`.
10. Run with `-bios us1`. Expect a Memory Error: that is normal before a RAM reset. RAM reset: turn the Operator key on (`O`), open the Main Door (`M`), wait about 16 s, hold Service and Cashout (`Y` + `C`) for about 6 s, then close the door.
11. The game then says "Please setup Game Machine Options". Steps from the project notes (a Multi-Line panel; the 9-line family was done this way): Jackpot key (`J`), Operator key (`O`, entry is sometimes missed, so retry), Service x4, Play 7 (`F`) x2 to reach Game Machine Options. Play 5 (`D`) sets the machine number. The Bet 2 (`W`) press that confirms each digit is **unverified**. Service x11 reaches Save. Close and reopen the Logic Door, then back out of the menus.

## 3. Games by type

Status means: **works** = scripted or manual setchip plus the game played a spin with fault bitmap 0 (local tests, 2026-11-06/07). **fails** = "EEPROM error" under every tried set-up. **untested** = no setchip tried. "Cashout presses" count from Multi-Line Game. Missouri = $2 buys 40 credits. Nevada = $0.05 is 1 credit.

| Machine ID line | Game type (Cashout presses) | Gameplay Setup | Jurisdiction | Example sets | Status |
|---|---|---|---|---|---|
| 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 lines. Bets per line not recorded (page says 10; unverified) | Missouri and Nevada | tiktorch, afterhrs, dbldolphu, geishajc, sunmoon | works (about 23 sets play from a tiktorch or own save; Missouri gave 22 plays and 1 inconclusive) |
| 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 lines, bets per line 5 | Missouri | atkgld, venice, blkmust, trsking, frogwld | works (12 of 12 in Missouri run) |
| 10 Credit Multiplier/15 Line Multiline | 15-Line Game (11) presumed | presumed 15 lines, bets per line 10 | unknown | gldgongu | untested |
| 10 Credit Multiplier/3125-Way (50 bet) Multiway | Super Reel Power Game (50) (21) | none recorded | Missouri | asnprinc, gwnn | works (gwnn set up by hand, passed the check) |
| 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | Bets per Line 8 (the default) | Nevada and Missouri | goldincu, deltabel, hrtantny, wingolyu | works |
| same line, 8 credit | as above | as above | none worked | bobdolly | **fails** (page Variation 05; Variation 5 did not help) |
| 5 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | Bets per Line 5 | Nevada and Missouri | centrion, shamnmgu, silkrda6u | works (max bet 125) |
| same line, 5 credit | as above | as above | none worked | 5dragce | **fails** (page Variation 05) |
| 8 Credit Multiplier/243-Way (25 bet) (pennyar, pompeia6u, pompeia6ua, whalecshu) | see Notes | not recorded | Missouri | pennyar, pompeia6u, pompeia6ua, whalecshu | works with the asnprinc save; correct type unverified |
| 4, 8 or 10 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | Number of Lines **"50 (2/credit)"** (Bet 1 x4), not plain "50" (x5). Bets per Line = page multiplier | Nevada and Missouri | 50lionsu, magforst, stardrftu (4); lineking, pelipeteu (8); bucaneeru (10) | works |
| 20 CR Multiplier/20 Line Multiline (maximum bet 500 credits) | Cashman Game (25) (12) | default (25 lines, bets per line 20) | **Nevada only** (Missouri fails; Variation 1 is not in the Missouri list) | arwincm, betzoo, eggsrccm, jefffox, jewelcm, pandpays | works (min bet 25, max 500) |
| 20 Credit Multiplier/25 Line Multiline | Multi-Line Game (0) | 25 lines, 20 credits per line | Missouri | deepfrzu | works (Variation 3 and 99 both worked) |
| 20 CR Multiplier/20 Line Multiline (maximum bet 500 credits) | Cashman Game presumed | untested | unknown | mysteycmu | untested |
| 5 CR Multiplier/20 Line Multiline (maximum bet 150 credits) | unknown | unknown | unknown | cinhell, mrwoou, zorroa6u | untested |
| 5 Credit Multiplier/20 Line Multiline; 2 Credit Multiplier/20 Line Multiline | unknown | unknown | unknown | scatmag3; wldafr | untested |
| 5 Credit Multiplier/25 Line Multiline | unknown | unknown | unknown | metestrm | untested |
| 5 Cr Multiplier/40 Cr Reel Power (200 Cr Max Bet); 5 Credit Multiplier/40 Credit Reelpower | Reel Power Game (40) is a guess (index 16) | unknown | unknown | buffalou; timbwolf | untested |
| 5 Credit Multiplier/5 Line Multiline | tried Arishinko, 5-Line Game, Multi-Line 5x5 | tried lines 5 x 1, 5 x 5, 5 x 10 | none worked | arish | **fails** (swhr2a6 shows the same line, untested) |
| 1 Credit Multiplier/3 Line Multiline | unknown | unknown | unknown | sunqndol | untested |

Notes on the table:

- The 9-line and 15-line saves for most sets were copies of one game's setchip save, followed by a RAM reset. Setting each game up by hand with the same type is the supported route.
- Contradiction: pennyar, pompeia6u, pompeia6ua and whalecshu show the 243-Way (25 bet) line, but they played from the `asnprinc` (Super Reel Power 50) save, while the other 243-way sets used Reel Power (25). Which type they need is not settled.
- Machine ID text alone does not always predict a working set-up. Variation (Miscellaneous Options, Bet 5 steps down, Bet 1 up) may be part of the check.
- The key panel changes with game type. For Reel Power, the tests used the panel button labelled Bet 5 to enter and clear. For Cashman the clear is Service plus a different button. Read the on-screen labels.
- Not tried: other setchip BIOSes (`set-us2` to `set-us4`), other jurisdictions for the failing games, and Payout Setup and Bill Validator options.

## 4. Errors

| Message | Meaning |
|---|---|
| EEPROM error! Run Setchip Program | The saved set-up does not match this game. Check Target Version (6.03.04), game type, lines and bets, jurisdiction. |
| YOU MUST OPEN THE LOGIC DOOR TO SAVE | Press `Z`, then Bet 5 again. |
| Memory Error (first run on `us1`) | Normal before the RAM reset. |
| Please setup Game Machine Options | The set-up passed. Do the Game Machine Options step (step 11). |
