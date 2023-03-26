package edu.fsu.omp.entity;

import lombok.Getter;
import lombok.Setter;

import javax.persistence.*;

@Entity
@Getter
@Setter
@Table(name="Product")
public class Product {
    @Id
    @GeneratedValue(strategy= GenerationType.IDENTITY)
    //@OneToOne(mappedBy="product")
    private Integer id;
    @Column
    private String category;
    @Column
    private String name;
    @Column
    private double price;
    @Column
    private int quantity;
    @Column
    private String address;
}
