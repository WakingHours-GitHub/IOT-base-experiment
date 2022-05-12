package base_experiment;

import javax.naming.InterruptedNamingException;
import java.security.PrivateKey;

public class User {
    private int id;
    private String name;
    private String gender;
    private double temperature;
    private double humidness;


    public User() {
    }
    public User(String[] s) {
        if(s[0].equals("null")){
            this.id = -1;
        }else {
            this.id = new Integer(s[0]);
        }

        this.name = s[1];
        this.gender = s[2];
        this.temperature = Double.valueOf(s[3]);
        this.humidness = Double.valueOf(s[4]);


    }

    public User(int id, String name, String gender, double temperature, double humidness) {
        this.id = id;
        this.name = name;
        this.gender = gender;
        this.temperature = temperature;
        this.humidness = humidness;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getGender() {
        return gender;
    }

    public void setGender(String gender) {
        this.gender = gender;
    }

    public double getTemperature() {
        return temperature;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public double getHumidness() {
        return humidness;
    }

    public void setHumidness(double humidness) {
        this.humidness = humidness;
    }

    @Override
    public String toString() {
        return "User{" +
                "id=" + id +
                ", name='" + name + '\'' +
                ", gender='" + gender + '\'' +
                ", temperature=" + temperature +
                ", humidness=" + humidness +
                '}';
    }
}
