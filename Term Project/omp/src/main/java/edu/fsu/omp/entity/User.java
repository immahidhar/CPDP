package edu.fsu.omp.entity;

import lombok.Getter;
import lombok.Setter;

import javax.persistence.*;

@Entity
@Getter
@Setter
@Table(name="User")
public class User {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    //@OneToOne(mappedBy="order")
    private Integer id;
    @Column(unique=true)
    private String username;
    @Column
    private String email;
    @Column
    private String address;
}
