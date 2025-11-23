function setStatus(text) {
  const el = document.getElementById('status');
  if (el) {
    el.textContent = text;
  }
}

async function sendCmd(c) {
  try {
    setStatus("Wysylam komendę: " + c + " ...");
    const res = await fetch("/cmd?c=" + encodeURIComponent(c));
    if (!res.ok) {
      setStatus("Blad HTTP: " + res.status);
      return;
    }
    const txt = await res.text();
    setStatus("Wyslano komendę: " + c + " (odpowiedz: " + txt + ")");
  } catch (e) {
    console.error(e);
    setStatus("Blad polaczenia: " + e);
  }
}
