import SideBar from '../components0/sideBar'
import ServiceStatus from '../components0/serviceTable'
import LineGraph from '../components0/lineGraph';
import StatCard from '../components0/statCard';

function Dashboard() {
    return (
        <>
            <h1>Dashboard</h1>
            <SideBar />
            <StatCard />
            <StatCard />
            <LineGraph />
            <ServiceStatus />
        </>
    )
}

export default Dashboard;