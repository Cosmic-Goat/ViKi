import {
  CategoryScale,
  Chart,
  ChartData,
  LinearScale,
  LineController,
  LineElement,
  PointElement,
  Title,
  Tooltip
} from 'chart.js';

Chart.register(
  LineElement,
  LineController,
  CategoryScale,
  LinearScale,
  PointElement,
  Title,
  Tooltip
);

async function main() {
  const getData = async (file: string) =>
    new Uint32Array(await (await (await fetch(file)).blob()).arrayBuffer());

  const bpmData = await getData('./bpm.dat');
  // const tempData = await getData('./temp.dat');
  // const oxData = await getData('./ox.dat');

  const config: ChartData = {
    labels: [1, 2, 3, 4, 5, 6, 7],
    datasets: [
      {
        label: 'My First Dataset',
        data: Array.from(bpmData),
        fill: false,
        borderColor: 'rgb(75, 192, 192)',
        tension: 0.1,
      },
    ],
  };

  const makeChart = (id: string, title: string, data: ChartData) =>
    new Chart(id, {
      type: 'line',
      data: data,
      options: {
        maintainAspectRatio: false,
        plugins: { title: { display: true, text: title } },
      },
    });

  const pulseChart = makeChart('pulse', 'Pulse', config);
  const bpmChart = makeChart('bpm', 'Heart Rate', config);
  const tempChart = makeChart('temp', 'Body Temperature', config);
  const o2Chart = makeChart('ox', 'Blood Oxygen Concentration', config);
}

window.addEventListener('load', main);
