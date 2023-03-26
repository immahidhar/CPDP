package edu.fsu.omp.data;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class ProductDTO {
    private Integer id;
    private Category category;
    private String name;
    private double price;
    private int quantity;
    private String address;

}
