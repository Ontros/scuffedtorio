# Building
cd build
cmake .
make

# Textury
Jelikož se mi textury nepodařilo dostat pod 10MB musíte si složku data stáhnout:
- Na emailu pavel.dohnalek@vsb.cz
- Discord link: https://cdn.discordapp.com/attachments/765179792286285834/1324474927369158788/data.zip?ex=677848e6&is=6776f766&hm=28e19a325e4a03280b02447c7f8181f21bf2a9cb07773a592b17aa553628b42b&
- Uschovna: https://www.uschovna.cz/zasilka/RAKU69WX6FR2FFGG-V72/

# Running
./scuffedtorio
valgrind --error-limit=no --suppressions=./sdl.supp --gen-suppressions=all --log-file=errors.log ./scuffedtorio

# Ovladani
WASD - Pohyb kamery
Scroll myši - Priblizeni/Oddaleni kamery
1 - Zeď
2 - Minning drill
3 - Gun turreta
4 - Laser turreta
5 - Flamethrower
Levé myšítko - Postavit budovu nebo kliknutí na tlačítko
Pravé myšítko - Zničit budovu
Q - Vybrat ze země nebo odebrat z ruky

# Hra
- Každá budova stojí určitý počet materiálů
- Wava začíná stisknutím tlačítka "Start wave"
- Wava se ukončí zabitím všech Enemy
- Gun turreta střílí iron
- Laser turreta střílí uran
- Flamethrower střílí uhlí
- Pokud turreta nemá dostatek materiálů v inventáři tak nestřílí
- Pokládejte Mining drill na orečka pro těžbu materiálů během wavy
- Když Enemy zničí rakovetové silo ve středu mapy tak hra skončí
- Zabíjení enemy přidává skóre a coiny za které se rozširuje concrete (místo kde se dá stavět)
- Začínáte s dostatkem materiálů pro gun turretu a 3 mining drilly