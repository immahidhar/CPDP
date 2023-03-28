package edu.fsu.omp.data;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class UserDTO {
    private Integer id;
    private String username;
    private String email;
    private String address;
}
