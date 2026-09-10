import "../styles/statCard.css";

interface StatCardProps {
    label: string;
    value: string | number;
    description?: string;
}

function StatCard({
    label,
    value,
    description
}: StatCardProps) {
    return (
        <section className="stat-card">
            <span className="stat-card-label">
                {label}
            </span>

            <strong className="stat-card-value">
                {value}
            </strong>

            {description && (
                <span className="stat-card-description">
                    {description}
                </span>
            )}
        </section>
    );
}

export default StatCard;