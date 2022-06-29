const url = `${location.protocol}//${location.host}`;
const checkButton = document.getElementById("btn-check");

async function loadData() {
  const request = await fetch(`${url}/app.php`).then((res) => res.json());
  const data = JSON.parse(request["m2m:cin"].con);
  const display = document.getElementById("data-display");

  // untuk display data ke halaman website
  display.childNodes[1].textContent = `${data.ph}`;
  display.childNodes[3].textContent = `${data.turbidity} NTU`;
  display.childNodes[5].textContent = `${data.temperature} ℃`;

  // display waktu
  document.getElementById("date-display").textContent = moment.unix(data.current_time).format("DD/MM/YYYY");
  document.getElementById("time-display").textContent = moment.unix(data.current_time).format("HH:mm:ss");
}
checkButton.addEventListener("click", loadData);
