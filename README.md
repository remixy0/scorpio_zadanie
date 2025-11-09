

<span style="font-size:22px;">**Zamiana układu współrzędnych na sferyczny**

Zacząłem od przekształcenia układu współrzędnych na układ sferyczny, ponieważ omawialiśmy to na wykładzie z fizyki. Zauważyłem przy tym podobieństwo między współrzędnymi sferycznymi a dwoma kątami, które wymagane są do sterowania silnikami. W tym celu wykorzystałem następujące wzory:

<span style="font-size:16px;">**_Współrzędne sferyczne (r, θ, φ) w zależności od kartezjańskich (x, y, z):_**

<span style="font-size:18px;">$r = \sqrt{x^2 + y^2 + z^2} $

<span style="font-size:18px;">$θ = \arctan2(y, x) $

<span style="font-size:18px;">$φ = \arccos\left(\frac{z}{r}\right) $

<br>


<span style="font-size:22px;">**Klasa Points**

Stworzyłem klasę Points, która pozwala na tworzenie obiektów reprezentujących punkty i zarządzanie nimi w programie. Klasa ta zawiera metody umożliwiające obliczenie wymaganych kątów do ustawienia silników w osiach X i Y.


<br>

<span style="font-size:22px;">**Ustawianie prędkości silników**

Do ustawiania prędkości silników wykorzystuję funkcję speedFormula, która określa zarówno kierunek, jak i wartość prędkości. Prędkość obliczana jest na podstawie funkcji matematycznej:

<br>

<span style="font-size:22px;">

 $127 \left( 1 - e^{-x/20} \right)$

</span>

<div style="display: flex; justify-content: center; gap: 20px;">
  <img src="assets/wykres.png" alt="Układ współrzędnych symulacji" width="500">
</div>


**_Silnik rozpędza się do maksymalnej prędkości, a w miarę zbliżania się do punktu docelowego prędkość gwałtownie spada, co umożliwia optymalne sterowanie ruchem._**


<br>

<span style="font-size:22px;">**Tryb kolejki (queue)**

Gdy program uruchamiany jest z argumentem queue, używana jest funkcja, która korzystając z poprzednich metod, kieruje silniki w stronę obiektu Point przekazanego do funkcji. Gdy silniki zbliżą się na odległość mniejszą niż ustawiony wcześniej procent błędu (errorPercent), punkt zostaje oznaczony jako osiągnięty, a funkcja przechodzi do kolejnych zadań.


<br>

<span style="font-size:22px;">**Tryb wywłaszczenia (preempt)**

Jeżeli program uruchomiony zostanie z argumentem preempt, silniki dążą do punktu currentPoint, który aktualizuje się w momencie pojawienia się nowego celu.


