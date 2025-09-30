**STUDENT: Drugea Casian**
**GRUPA: 312CA**

## Segregated Free Lists - SDA Tema I

### Implementare

Implementarea aleasa de mine include folosirea unui vector de liste dublu inlantuite numit in continuare SFL, iar pentru a stoca blockurile de memorie alocata am  folosit o lista simplu inlantuita generica numita in continuare SFL. La inceput se populeza fiecare lista din SFL cu numar de blockuri egal cu numarul de bytes corespunzator fiecarei liste impartit la dimensiune blockului unei liste, care initial sunt puteri ale lui 2.

### Comenzi

* `MALLOC`: Se determina daca exista o lista de dimensiune a blockurilor >= nr. de 
bytes alocati. In caz afirmativ se scoate head-ul liste respective si se insereaza in 
ML astfel incat ML ramane ordonat crescator dupa adresa. In cazul in care 
dimensiunea blockului alocat este strict mai mare decat nr de bytes ce trebuie
alocati atunci se face o fragmentare. In continuare fragmentul trebuie introdus
in lista corespunzatoare dimensiunii lui. Verficam daca exista o lista corespunzatoare
dimensiunii lui, in caz afirmativ introducem blocul in acea lista astfel incat 
lista ramane sortata crescator dupa adresa. Daca nu exista o lista de acea dimensiune
creem una noua, ea fiind introdusa in SFL astfel incat listele vor fi ordonate 
crescator dupa nr de bytes pe care il au blockurile corespunzatoare. Asadar se 
realoca vectorul SFL , crescandu-i dimensiunea cu 1. Dupa ce am creat noua lista
setam head-ul ca fiin blockul ce trebuie introdus.

* FREE: Se determina daca a fost alocata adresa respectiva iar in caz afirmativ
blocul respectiv este eliminat din ML si adaugat in lista corespunzatoare din SFL
flosind acelasi algoritm ca la adaugarea unui fragment in SFL.

* `WRITE`: Initial se verifica  daca se pot scrie adresa + nr_bytes asigurandu-ne
ca zona de memorie este continua(adresa nod curent + nr_bytes nod curent == 
adresa nodului urmator). In caz afirmativ se parcurge ML incepan cu adresa gasita
si se scrie in campul "content" alocat in prealabil de dimensiunea nr_bytes
corspunzator blocului sau cu nr de bytes ce mai trebuie scrisi daca acestia sunt
mai putini decat dimensiunea blockului. Daca nu se poate scrie se afiseaza 
mesajul de eroare aferent si se face un `DUMP_MEMORY` care afiseaza statusul
memoriei dupa care se opreste executia programului.

* `READ`: Similar cu `WRITE` se cauta blockul cu adresa citita, se verifica daca 
exista zona de memorie adresa + nr_bytes alocata si continua, iar in caz afirmativ
se afiseaza nr_bytes octeti. In caz contrar se face dump memoriei asemenea 
functionalitatii `WRITE`.

* `DUMP_MEMORY`: Se afiseaza statusul memoriei alocate si libere pe baza unor functii
care intereaza prin liste, sau pe baza unor campuri ale structurilor asociate SFL sau ML.

* `DESTROY_MEMORY`: Se da free fiecarui nod din fiecare lista din SFL si din ML. In cazul ML care este o structura de date generica se elibereaza si memoria aferenta camplui content, daca acesta a fost alocat in prealabil(daca s-a scris la adresa blocului minim un byte).

### BONUS
* La inceput fiecare nod din fiecare lista din SFL primeste un index unic.
* De fiecare data cand se face malloc introducem atat blockul alocat cat si fragmentul rezultat cu un index egal cu indexul blockului parinte + SET(un define suficent de  mare).
* In cazul in care se face malloc si nu se fecetueaza nicio fragmentare, blocul alocat va avea acelasi index. In momentul in care se se da free unui bloc, pentru fiecare nod din fiecare lista SFL se va cauta un block cu acelasi index. Daca se gaseste cele 2 blockuri sunt eliminate si se introduce un block nou, practic blockul care a fost fragmentat in trecut cu index egal index - SET. Aceasta operatie se repeta atata timp cat exista 2 noduri cu acelasi indice, si se  efectueaza la fiecare FREE.

### CONCLUZII
* Cea mai grea parte a temei mi s-a parut bonusul, iar ceea ce mi-a placut a fost insasi tematica temei, aceea de a-ti propiul alocator de memorie folosind notiunile proaspat invatate la cursul de SDA. 
* As fi putut imbunatati timpul de executie al programului cu precadere pentru functionalitatea `DUMP_MEMORY` unde as fi putut retine modificarile memoriei de-a lungul programului pentru mai multe statistici decat am facut-o in actuala implementare.
