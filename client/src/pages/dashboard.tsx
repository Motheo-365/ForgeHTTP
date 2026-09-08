import SideBar from '../components/sideBar'
import ServiceStatus from '../components/serviceTable'
import LineGraph from '../components/lineGraph';
import StatCard from '../components/statCard';

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