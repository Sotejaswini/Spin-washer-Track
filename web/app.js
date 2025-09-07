const API = window.__API_BASE__ || "http://localhost:8080";

const el = (id) => document.getElementById(id);
const bookForm = el("bookForm");
const bookMsg = el("bookMsg");
const machines = el("machines");
const queue = el("queue");
const filterWingInput = el("filterWing");
const healthBadge = el("healthBadge");
const doneSound = el("doneSound");
const logsPre = el("logs");

let filterWing = "";
let lastStatus = { machines: [], queue: [] };

async function pingHealth() {
  try {
    const r = await fetch(`${API}/health`);
    healthBadge.textContent = r.ok ? "API: healthy" : "API: unreachable";
    healthBadge.style.color = r.ok ? "#10b981" : "#ef4444";
  } catch {
    healthBadge.textContent = "API: unreachable";
    healthBadge.style.color = "#ef4444";
  }
}
pingHealth();

bookForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const name = el("name").value.trim();
  const wing = el("wing").value.trim();
  const duration = Number(el("duration").value);
  if (!name || !wing || !duration) {
    bookMsg.textContent = "Please fill all fields.";
    return;
  }
  try {
    const url = new URL(`${API}/book`);
    url.searchParams.set("name", name);
    url.searchParams.set("wing", wing);
    url.searchParams.set("duration", String(duration));
    const r = await fetch(url, { method: "POST" });
    const txt = await r.text();
    bookMsg.textContent = r.ok ? "✅ Booking accepted." : `❌ ${txt}`;
  } catch {
    bookMsg.textContent = "❌ Failed to reach API.";
  }
});

el("applyFilter").addEventListener("click", () => {
  filterWing = filterWingInput.value.trim().toLowerCase();
  render();
});
el("clearFilter").addEventListener("click", () => {
  filterWing = "";
  filterWingInput.value = "";
  render();
});
el("refreshNow").addEventListener("click", refresh);
el("tailLogs").addEventListener("click", async () => {
  try {
    const r = await fetch(`${API}/logs?n=50`);
    const txt = await r.text();
    logsPre.textContent = txt;
    logsPre.hidden = false;
  } catch {
    logsPre.textContent = "Failed to load logs.";
    logsPre.hidden = false;
  }
});

function formatSecs(s) {
  const sec = Math.max(0, Number(s) | 0);
  const m = Math.floor(sec / 60),
    r = sec % 60;
  return m > 0 ? `${m}m ${r}s` : `${r}s`;
}

function render() {
  machines.innerHTML = "";
  queue.innerHTML = "";

  lastStatus.machines.forEach((m) => {
    const card = document.createElement("div");
    card.className = "card";
    const badge = m.busy
      ? `<span class="badge busy">BUSY</span>`
      : `<span class="badge free">FREE</span>`;
    card.innerHTML = `
      <h3>Machine #${m.id} ${badge}</h3>
      <div class="kv"><span>User</span><span>${m.user || "-"}</span></div>
      <div class="kv"><span>Wing</span><span>${m.wing || "-"}</span></div>
      <div class="timer">${formatSecs(m.remaining)}</div>
    `;
    machines.appendChild(card);
  });

  const q = lastStatus.queue
    .filter((qi) => !filterWing || qi.wing.toLowerCase() === filterWing)
    .sort((a, b) => a.seq - b.seq);

  if (q.length === 0) {
    queue.innerHTML = `<div class="small">No waiting users${
      filterWing ? ` in wing "${filterWing}"` : ""
    }.</div>`;
  } else {
    q.forEach((qi) => {
      const div = document.createElement("div");
      div.className = "rowItem";
      div.innerHTML = `<div><strong>${qi.name}</strong></div>
      <div class="small">Wing: ${qi.wing} • Duration: ${qi.duration}s • Seq: ${qi.seq}</div>`;
      queue.appendChild(div);
    });
  }
}

function detectDone(prev, curr) {
  const prevById = {};
  prev.machines.forEach((m) => (prevById[m.id] = m));
  curr.machines.forEach((m) => {
    const p = prevById[m.id];
    if (p && p.busy && !m.busy) {
      const msg = `Machine #${m.id}: Done! ${
        p.user ? p.user + "," : ""
      } please put in clothes.`;
      try {
        doneSound.currentTime = 0;
        doneSound.play().catch(() => {});
      } catch {}
      alert(msg);
      if ("Notification" in window) {
        if (Notification.permission === "granted")
          new Notification("Washer Done", { body: msg });
        else if (Notification.permission !== "denied")
          Notification.requestPermission().then((v) => {
            if (v === "granted") new Notification("Washer Done", { body: msg });
          });
      }
    }
  });
}

async function refresh() {
  try {
    const r = await fetch(`${API}/status`);
    const data = await r.json();
    detectDone(lastStatus, data);
    lastStatus = data;
    render();
  } catch {
    healthBadge.textContent = "API: unreachable";
    healthBadge.style.color = "#ef4444";
  }
}
setInterval(refresh, 1000);
refresh();
