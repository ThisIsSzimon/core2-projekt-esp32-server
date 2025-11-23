function setStatus(text) {
  const el = document.getElementById('status');
  if (el) {
    el.textContent = text;
  }
}

async function sendCmd(c) {
  try {
    const res = await fetch("/cmd?c=" + encodeURIComponent(c));
    if (!res.ok) {
      setStatus("Blad HTTP: " + res.status);
      return;
    }
    const txt = await res.text();
    console.log("CMD", c, "resp:", txt);
  } catch (e) {
    console.error(e);
    setStatus("Blad polaczenia: " + e);
  }
}

/**
 * Podpina do przycisku logikę:
 * - wciśnięcie (mysz / dotyk)  -> cmdDown (TYLKO raz)
 * - puszczenie / wyjechanie    -> cmdUp (TYLKO jeśli wcześniej był start)
 */
function attachHoldButton(btnId, cmdDown, cmdUp, label) {
  const btn = document.getElementById(btnId);
  if (!btn) {
    console.error("Brak przycisku:", btnId);
    return;
  }

  let isActive = false; // czy ten przycisk aktualnie "trzyma" silnik

  function start(e) {
    e.preventDefault();
    if (isActive) return;           // już aktywny, ignoruj kolejne
    isActive = true;

    setStatus("START: " + label + " (cmd: " + cmdDown + ")");
    sendCmd(cmdDown);
  }

  function stop(e) {
    e.preventDefault();
    if (!isActive) return;          // nie był aktywny -> nie wysyłamy STOP
    isActive = false;

    setStatus("STOP: " + label + " (cmd: " + cmdUp + ")");
    sendCmd(cmdUp);
  }

  // Mysz
  btn.addEventListener('mousedown', start);
  btn.addEventListener('mouseup', stop);
  btn.addEventListener('mouseleave', stop);

  // Dotyk (telefon / tablet)
  btn.addEventListener('touchstart', start);
  btn.addEventListener('touchend', stop);
  btn.addEventListener('touchcancel', stop);

  // Wyłącz menu kontekstowe (prawy klik / długie tapnięcie)
  btn.addEventListener('contextmenu', e => e.preventDefault());
}

window.addEventListener('load', () => {
  // Most: A/a + STOP = X
  attachHoldButton('btnBridgeUp',   'A', 'X', 'Most: Góra');
  attachHoldButton('btnBridgeDown', 'a', 'X', 'Most: Dół');

  // Pojazd na moście: B/b + STOP = Y
  attachHoldButton('btnCarBridgeFwd',  'B', 'Y', 'Pojazd na moście: Przód');
  attachHoldButton('btnCarBridgeBack', 'b', 'Y', 'Pojazd na moście: Tył');

  // Pojazd na biurku: C/c + STOP = Z
  attachHoldButton('btnCarDeskFwd',  'C', 'Z', 'Pojazd na biurku: Przód');
  attachHoldButton('btnCarDeskBack', 'c', 'Z', 'Pojazd na biurku: Tył');
});
