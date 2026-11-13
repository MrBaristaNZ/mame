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

Status means: **works** = the setchip was run on that game, and the game took coins and played a spin with fault bitmap 0 (local tests, 2026-11-06/07, and a survey of every US set on 2026-11-13). **fails** = "EEPROM error" under every set-up tried. Section 5 lists every US set one by one. "Cashout presses" count from Multi-Line Game. Missouri = $2 buys 40 credits. Nevada = $0.05 is 1 credit.

| Machine ID line | Game type (Cashout presses) | Gameplay Setup | Jurisdiction | Example sets | Status |
|---|---|---|---|---|---|
| 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 lines. Bets per line not recorded (page says 10; unverified) | Missouri and Nevada | tiktorch, afterhrs, dbldolphu, geishajc, sunmoon | works (about 23 sets play from a tiktorch or own save; Missouri gave 22 plays and 1 inconclusive) |
| 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 lines, bets per line 5 | Missouri | atkgld, venice, blkmust, trsking, frogwld | works (12 of 12 in Missouri run) |
| 10 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 lines, bets per line 10 (the default) | Missouri | gldgongu | works (a progressive game: after the RAM reset a Progressive Jackpot Edit screen appears; Bet 3 saves it) |
| 10 Credit Multiplier/3125-Way (50 bet) Multiway | Super Reel Power Game (50) (21) | none recorded | Missouri | asnprinc, gwnn | works (gwnn set up by hand, passed the check) |
| 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | Bets per Line 8 (the default) | Nevada and Missouri | goldincu, deltabel, hrtantny, wingolyu | works |
| same line, 8 credit | as above | as above | none worked | bobdolly | **fails** (the EEPROM check rejects every set-up tried: Reel Power 25 with Variation 5, Link Progressive, Ante 5) |
| 5 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | Bets per Line 5 | Nevada and Missouri | centrion, shamnmgu, silkrda6u | works (max bet 125) |
| same line, 5 credit | as above | as above | none worked | 5dragce | **fails** (Reel Power 25/30/40/50, Super Reel Power 30/50, Variation 99 and 5, Ante 5: all rejected) |
| 8 Credit Multiplier/243-Way (25 bet) (pennyar, pompeia6u, pompeia6ua, whalecshu) | Reel Power Game (25) (14) | Bets per Line 8 (the default) | Missouri | pennyar, pompeia6u, pompeia6ua, whalecshu | works (they also play on the Super Reel Power 50 save) |
| 4, 8 or 10 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | Number of Lines **"50 (2/credit)"** (Bet 1 x4), not plain "50" (x5). Bets per Line = page multiplier | Nevada and Missouri | 50lionsu, magforst, stardrftu (4); lineking, pelipeteu (8); bucaneeru (10) | works |
| 20 CR Multiplier/20 Line Multiline (maximum bet 500 credits) | Cashman Game (25) (12) | default (25 lines, bets per line 20) | **Nevada only** (Missouri fails; Variation 1 is not in the Missouri list) | arwincm, betzoo, eggsrccm, jefffox, jewelcm, pandpays | works (min bet 25, max 500) |
| 20 Credit Multiplier/25 Line Multiline | Multi-Line Game (0) | 25 lines, 20 credits per line | Missouri | deepfrzu | works (Variation 3 and 99 both worked) |
| 20 CR Multiplier/20 Line Multiline (maximum bet 500 credits) | Cashman Game (25) (12) | default | **Missouri**, Variation 99 (Nevada Variation 1, the arwincm set-up, is rejected) | mysteycmu | works |
| 5 CR Multiplier/20 Line Multiline (maximum bet 150 credits) | Cashman Game (30) (13) | default (30 lines, bets per line 20) | Nevada, Variation 1 | cinhell; mrwoou, zorroa6u | cinhell works; mrwoou and zorroa6u **fail** (they reject the cinhell set-up, Missouri, and Multi-Line 20 x 10) |
| 5 Credit Multiplier/20 Line Multiline; 2 Credit Multiplier/20 Line Multiline | Multi-Line Game (0) | 20 lines (Bet 1 x2), bets per line 10 (default) | Missouri | scatmag3; wldafr | scatmag3 works; wldafr **fails** (rejects the same set-up, Nevada, and Variation changes) |
| 5 Credit Multiplier/25 Line Multiline | Multi-Line Game (0) | 25 lines (Bet 1 x3), bets per line 10 (default; 3 is rejected) | Missouri | metestrm | works |
| 5 Cr Multiplier/40 Cr Reel Power (200 Cr Max Bet); 5 Credit Multiplier/40 Credit Reelpower | Reel Power Game (40) (16) | Bets per Line 5 (Bet 2 once from 10) | Missouri | buffalou; timbwolf | works. timbwolf's game panel differs: RAM reset is Main Door + Bet 2 + Bet 1 |
| 5 Credit Multiplier/5 Line Multiline | Multi-Line Game (0) (5-Line Game is rejected) | 5 lines (Bet 2 once), bets per line 10 (default) | Missouri | swhr2a6; arish | swhr2a6 works; arish **fails** (rejects the same set-up, 5-Line Game and Arishinko) |
| 1 Credit Multiplier/3 Line Multiline | tried 3-Line Game (1) and 3-Line Game (Dollar Storm) | 3 lines x 1 | Missouri and Nevada tried | sunqndol | **fails**. Dollar Storm can only be set up through the UPI key panel |
| (multi-game, no Machine ID page) | BaseX / Multi-Game (UPI) clears its "Process startup" stop | | Missouri and Nevada tried | pwsuper | **fails** (reaches its game-select screen, then "EEPROM error") |

Notes on the table:

- The setchip's output does not depend on the game: the same settings give a byte-identical `eeprom0` on different sets (checked on every family, 2026-11-13). So a save copied from another game of the same type is the same set-up, and a RAM reset is all it needs.
- Bets per Line is a maximum, and must be at least the "N Credit Multiplier" on the Machine Identification page: metestrm rejects 25 lines x 3 and accepts 25 x 10. The default is always high enough on the sets above.
- Changing Lines or Bets per Line can change the setchip key panel (the labels on screen change). Read the labels after each change.
- arish, wldafr, mrwoou and zorroa6u reject a set-up that is byte-identical to one that works for another game with the same ID line (swhr2a6, scatmag3, cinhell). So these games check something beyond type, lines, bets, jurisdiction and Variation. What that is is not known yet.
- Machine ID text alone does not always predict a working set-up. Variation (Miscellaneous Options, Bet 5 steps down, Bet 1 up) may be part of the check.
- The key panel changes with game type. For Reel Power, the tests used the panel button labelled Bet 5 to enter and clear. For Cashman the clear is Service plus a different button. Read the on-screen labels.
- Not tried: other setchip BIOSes (`set-us2` to `set-us4`), and Payout Setup and Bill Validator options.

## 4. Errors

| Message | Meaning |
|---|---|
| EEPROM error! Run Setchip Program | The saved set-up does not match this game. Check Target Version (6.03.04), game type, lines and bets, jurisdiction. |
| YOU MUST OPEN THE LOGIC DOOR TO SAVE | Press `Z`, then Bet 5 again. |
| Memory Error (first run on `us1`) | Normal before the RAM reset. |
| Please setup Game Machine Options | The set-up passed. Do the Game Machine Options step (step 11). |

## 5. Every US set

From a survey of all 85 US sets on 2026-11-13: 69 work, 8 fail, 8 are bad dumps that can't boot. Every row uses Target Version 6.03.04, the `_41` to `_27` copy, the RAM reset and Game Machine Options (section 2). Variation is the setchip default unless the row says otherwise (99, or 1 for Cashman on Nevada).

| Set | Machine ID line | Game type (Cashout presses) | Lines | Bets per line | Jurisdiction | Variation | Result |
|---|---|---|---|---|---|---|---|
| 50lionsu | 4 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 4 | Missouri ($2 = 40) | 99 | works |
| 5dragce | 5 Credit Multiplier/243-Way (25 bet) Multiway |  |  |  |  |  | **fails**: EEPROM error |
| adonisa6 | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| afterhrs | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| apocof |  |  |  |  |  |  | bad dump, can't boot |
| arabnga6u | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| arish | 5 Credit Multiplier/5 Line Multiline |  |  |  |  |  | **fails**: EEPROM error |
| arwincm | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works. Missouri fails (Variation 1 not in its list) |
| asnprinc | 10 Credit Multiplier/3125-Way (50 bet) Multiway | Super Reel Power Game (50) (21) | 3125 ways (fixed) | default | Missouri ($2 = 40) | 99 | works |
| atkgld | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| betzoo | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works |
| blkmust | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| bmbugs | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| bobdolly | 8 Credit Multiplier/243-Way (25 bet) Multiway |  |  |  |  |  | **fails**: EEPROM error |
| bucaneeru | 10 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 10 | Missouri ($2 = 40) | 99 | works |
| buffalou | 5 Cr Multiplier/40 Cr Reel Power (200 Cr Max Bet) | Reel Power Game (40) (16) | 40 credit reel power (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works (new) |
| centrion | 5 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 5 (Bet 2 x3 from 8) | Missouri ($2 = 40) | 99 | works |
| cinhell | 5 CR Multiplier/20 Line Multiline(maximum bet 150 credits) | Cashman Game (30) (13) | 30 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works (new) |
| cryspriu | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| dagenta6 | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| dbldolphu | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| deepfrzu | 20 Credit Multiplier/25 Line Multiline | Multi-Line Game (0) | 25 | 20 | Missouri ($2 = 40) | 99 | works. Variation 3 and 99 both worked |
| deltabel | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works |
| dimeyece | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| dimeyeu | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| eaglrock | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| eastland | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| efce |  |  |  |  |  |  | bad dump, can't boot |
| eggsrccm | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works |
| flamolce | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| flamoljc | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| frogwld | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| gbmoney | N/A | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| geishajc | N/A | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| gldgongu | 10 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 10 (default) | Missouri ($2 = 40) | 99 | works (new). Progressive game: after the RAM reset a Progressive Jackpot Edit screen appears; Bet 3 Credits saves it and goes on |
| goldincu | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works |
| gwnn | 10 Credit Multiplier/3125-Way (50 bet) Multiway | Super Reel Power Game (50) (21) | 3125 ways (fixed) | default | Missouri ($2 = 40) | 99 | works |
| hlntroyu | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| hrtantny | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works |
| jefffox | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works. Same set-up as arwincm |
| jewelcm | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works |
| lemrol | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| lineking | 8 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 8 | Missouri ($2 = 40) | 99 | works |
| lovestuk |  |  |  |  |  |  | bad dump, can't boot |
| magforst | 4 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 4 | Missouri ($2 = 40) | 99 | works |
| magmerm | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| metestrm | 5 Credit Multiplier/25 Line Multiline | Multi-Line Game (0) | 25 (Bet 1 x3) | 10 (default) | Missouri ($2 = 40) | 99 | works (new). 25 x 3 rejected (bets per line below the page's 5) |
| mrwoou | 5 CR Multiplier / 20 Line Multiline ( max bet 150 credits ) |  |  |  |  |  | **fails**: EEPROM error |
| mystarr | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| mysteycmu | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Missouri ($2 = 40) | 99 | works (new). Nevada Variation 1 (the arwincm setup) is rejected; Missouri Variation 99 works |
| opsesam |  |  |  |  |  |  | bad dump, can't boot |
| pandpays | 20 CR Multiplier/20 Line Multiline(maximum bet 500 credits) | Cashman Game (25) (12) | 25 lines CM (20 + Cashman) | 20 (default) | Nevada (5c = 1) | 1 | works |
| parislgtu | N/A | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| pcockmagu | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| pelipeteu | 8 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 8 | Missouri ($2 = 40) | 99 | works |
| pengpyce | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| pennyar | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works. also plays on the asnprinc Super Reel Power (50) save |
| pompeia6u | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works. also plays on the asnprinc Super Reel Power (50) save |
| pompeia6ua | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works. also plays on the asnprinc Super Reel Power (50) save |
| pwsuper | (multi-game; no Machine ID page reached) |  |  |  |  |  | **fails**: EEPROM error |
| roadtrip |  |  |  |  |  |  | bad dump, can't boot |
| scatmag3 | 5 Credit Multiplier/20 Line Multiline | Multi-Line Game (0) | 20 (Bet 1 x2) | 10 (default) | Missouri ($2 = 40) | 99 | works (new) |
| sctmagce | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| sealdeal | N/A | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| shamnmgu | 5 Credit Multiplier/243-Way (25 bet) Multiway (125Cr Max Bet) | Reel Power Game (25) (14) | 243 ways (fixed) | 5 (Bet 2 x3 from 8) | Missouri ($2 = 40) | 99 | works |
| shorthng | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| silkrda6u | 5 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 5 (Bet 2 x3 from 8) | Missouri ($2 = 40) | 99 | works |
| stardrftu | 4 Credit Multiplier/50 Line Multiline | Multi-Line Game (0) | "50 (2/credit)" (Bet 1 x4) | 4 | Missouri ($2 = 40) | 99 | works |
| statsymb | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| sunmonbb |  |  |  |  |  |  | bad dump, can't boot |
| sunmoon | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| sunmoona | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| sunqndol | 1 Credit Multiplier/3 Line Multiline |  |  |  |  |  | **fails**: EEPROM error |
| swhr2a6 | 5 Credit Multiplier/5 Line Multiline | Multi-Line Game (0) | 5 (Bet 2 x1) | 10 (default) | Missouri ($2 = 40) | 99 | works (new). 5-Line Game (1) is rejected |
| tiktorch | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| timbwolf | 5 Credit Multiplier/40 Credit Reelpower | Reel Power Game (40) (16) | 40 credit reel power (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works (new). Game panel differs: operator menu Bet 1 next / Bet 3 choose / Service back; RAM reset = Main Door + Bet 2 + Bet 1 |
| trsking | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| venice | 5 Credit Multiplier/15 Line Multiline | 15-Line Game (11) | 15 (fixed) | 5 (Bet 2 x1 from 10) | Missouri ($2 = 40) | 99 | works |
| whalecshu | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works. also plays on the asnprinc Super Reel Power (50) save |
| whalecshua |  |  |  |  |  |  | bad dump, can't boot |
| whtwater | 10 Credit Multiplier/9 Line Multiline | Multi-Line Game (0) | 9 (default) | 10 (default) | Missouri ($2 = 40) | 99 | works |
| wingolyu | 8 Credit Multiplier/243-Way (25 bet) Multiway | Reel Power Game (25) (14) | 243 ways (fixed) | 8 (default) | Missouri ($2 = 40) | 99 | works |
| wldafr | 2 Credit Multiplier/20 Line Multiline |  |  |  |  |  | **fails**: EEPROM error |
| wldpanth |  |  |  |  |  |  | bad dump, can't boot |
| zorroa6u | 5 CR Multiplier / 20 Line Multiline ( max bet 150 credits ) |  |  |  |  |  | **fails**: EEPROM error |
