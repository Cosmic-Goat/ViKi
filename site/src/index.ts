import Chart from 'chart.js/auto';

const getData = async (file: string) =>
  await (await (await fetch(file)).blob()).arrayBuffer();

const range = (start: number, end: number, n: number) =>
  Array.from({ length: n }, (_, i) => start + i * ((end - start) / (n - 1)));

const makeChart = (
  id: string,
  title: string,
  labels: any[],
  data: number[] | Uint32Array | Float32Array,
  color: string,
  unit = '',
  time = '',
  min = 0,
) =>
  new Chart(id, {
    type: 'line',
    data: {
      labels: labels,
      datasets: [
        {
          label: title,
          data: Array.from(data),
          fill: false,
          borderColor: color,
          tension: 0.1,
        },
      ],
    },
    options: {
      scales: {
        yAxis: {
          min: min,
          title: { display: true, text: unit },
        },
        xAxis: {
          title: { display: true, text: time },
        },
      },
      maintainAspectRatio: false,
      plugins: {
        legend: { display: false },
        title: { display: true, text: title, font: { size: 16 } },
      },
    },
  });

async function main() {
  const bpmData = new Float32Array(await getData('./bpm.dat'));
  const pulseData = new Uint32Array(await getData('./pulse.dat'));
  const tempData = new Float32Array(await getData('./temp.dat'));
  const o2Data = new Float32Array(await getData('./o2.dat'));
  const time = range(60, 0, 7);

  const bpmChart = makeChart(
    'bpm',
    'Heart Rate',
    time,
    bpmData,
    '#f4a261',
    'Beats Per Minute',
    'Minutes Ago',
    0,
  );
  const pulseChart = makeChart(
    'pulse',
    'Pulse',
    range(0, (pulseData.length - 1) / 20, pulseData.length).map((i: number) =>
      i.toFixed(2),
    ),
    pulseData,
    '#e76f51',
    'Raw Value',
    'Seconds',
  );
  const tempChart = makeChart(
    'temp',
    'Body Temperature',
    time,
    tempData,
    '#2a9d8f',
    'Degrees Celcius',
    'Minutes Ago',
  );
  const o2Chart = makeChart(
    'ox',
    'Blood Oxygen Concentration',
    time,
    o2Data,
    '#264653',
    "Percentage",
    "Minutes Ago",
    75
  );
}

window.addEventListener('load', main);
