function ServiceTable() {
    return (
        <section>
            <input type="search" placeholder="Search"/>
            <input type="button" placeholder="Download" />
            
            <table>
                <thead>
                    <tr>
                        <th>Table</th>
                        <th>Service</th>
                        <th>URL</th>
                        <th>Uptime</th>
                        <th>Response</th>
                    </tr>
                </thead>

                <tbody>
                    <tr>
                        <td>Up</td>
                        <td>Mark Website</td>
                        <td>https://example.com</td>
                        <td>100%</td>
                        <td>142ms</td>
                    </tr>

                    <tr>
                        <td>Up</td>
                        <td>API Server</td>
                        <td>https://api.example.com</td>
                        <td>99.99%</td>
                        <td>89ms</td>
                    </tr>

                    <tr>
                        <td>Up</td>
                        <td>CDN</td>
                        <td>https://cdn.example.com</td>
                        <td>100%</td>
                        <td>45ms</td>
                    </tr>

                    <tr>
                        <td>Up</td>
                        <td>Database</td>
                        <td>https://database.example.com</td>
                        <td>99.95%</td>
                        <td>12ms</td>
                    </tr>

                    <tr>
                        <td>Degraded</td>
                        <td>Storage Service</td>
                        <td>https://storage.example.com</td>
                        <td>98.2%</td>
                        <td>256ms</td>
                    </tr>

                    <tr>
                        <td>Up</td>
                        <td>EMail Service</td>
                        <td>https://mail.example.com</td>
                        <td>99.98%</td>
                        <td>178ms</td>
                    </tr>
                </tbody>
            </table>
        </section>
    );
}

export default ServiceTable;