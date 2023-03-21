package edu.fsu.omp.entity;

import javax.persistence.*;

@Entity
public class User {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    private Integer id;
    @Column(unique=true)
    private String username;
    @Column
    private String email;
    @Column
    private String address;
}
