import {
  CategoryScale,
  Chart,
  LinearScale,
  LineController,
  LineElement,
  PointElement,
} from 'chart.js';

Chart.register(
  LineElement,
  LineController,
  CategoryScale,
  LinearScale,
  PointElement,
);

async function main() {
  const getData = async (file: string) =>
    new Uint32Array(await (await (await fetch(file)).blob()).arrayBuffer());

  const bpmData = await getData('./bpm.dat');
  // const tempData = await getData('./temp.dat');
  // const oxData = await getData('./ox.dat');

  const config = {
    labels: [1, 2, 3, 4, 5, 6, 7],
    datasets: [
      {
        label: 'My First Dataset',
        data: bpmData,
        fill: false,
        borderColor: 'rgb(75, 192, 192)',
        tension: 0.1,
      },
    ],
  };

  const bpmChart = new Chart('bpm', {
    type: 'line',
    data: config,
    options: {},
  });

  const tempChart = new Chart('temp', {
    type: 'line',
    data: config,
    options: {},
  });

  const oxChart = new Chart('ox', {
    type: 'line',
    data: config,
    options: {},
  });
}

window.addEventListener('load', main);
